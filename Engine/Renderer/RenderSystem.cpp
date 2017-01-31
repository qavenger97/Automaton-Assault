/*!
 * \file RenderSystem.cpp
 *
 * \author Shiyang Ao
 * \date November 2016
 *
 * 
 */
#include "pch.h"

#include "RenderSystem.h"
#include "D3DUtil.h"
#include "DebugRenderer.h"
#include "Component/Transform.h"
#include "RenderDevice.h"
#include "System/CameraSystem.h"

#include "VertexSignature/ColorVertexSignature.csh"
#include "VertexSignature/FontVertexSignature.csh"
#include "VertexSignature/MeshVertexSignature.csh"
#include "VertexSignature/SkinnedMeshVertexSignature.csh"
#include "Component/Animation.h"
#include "System/GPUParticleSystem.h"
#include "DevTextRenderer.h"
#include "Core/Profiler.h"
#include "Core/FileUtils.h"
#include "Core/TimeManager.h"

#include "Texture.h"
#include "DebugConst.h"

#define REPORT_LIVE_DIRECTX 0

static bool s_DrawDebugLightInfo = false;
static bool s_DrawRenderComponentAabb = false;
static bool s_DrawRenderingStatistics = false;
static bool s_DrawProfilerInfo = false;
static bool s_bVsync = false;
extern bool g_UsePreDepthPass;
static bool s_UseDebugSimplePixelShader = false;
static bool s_EnablePostProcessing = true;
//static float s_InnerCone = 0.97f;
//static float s_OuterCone = 0.9f;

Hourglass::Texture* g_ScreenOverlay = nullptr;
Color g_ScreenOverlayColor;

bool Hourglass::RenderSystem::Init(HWND hWnd, int width, int height, bool useMsaa)
{
	INIT_COMPONENT_POOL(PrimitiveRenderer);
	INIT_COMPONENT_POOL(MeshRenderer);
	INIT_COMPONENT_POOL(SkinnedMeshRenderer);
	INIT_COMPONENT_POOL(Light);

	m_ClientWidth = width;
	m_ClientHeight = height;
	m_MsaaEnabled = useMsaa;

	UINT createDeviceFlags = 0;

#if defined(DEBUG) || defined(_DEBUG)
	createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	// Create DXGI factory interface
	ComPtr<IDXGIFactory1> dxgiFactory = 0;
	HR(CreateDXGIFactory1(__uuidof(IDXGIFactory1), (void**)dxgiFactory.GetAddressOf()));

	// Enumerate adapters
	IDXGIAdapter* dxgiAdapter = 0;
	std::vector<IDXGIAdapter*> vAdapters;

	// Select best adapter with most VRAM
	UINT bestAdapterIndex = 0;
	size_t bestAdapterMem = 0;

	for (UINT i = 0;
		dxgiFactory->EnumAdapters(i, &dxgiAdapter) != DXGI_ERROR_NOT_FOUND;
		++i)
	{
		vAdapters.push_back(dxgiAdapter);
		DXGI_ADAPTER_DESC desc;
		dxgiAdapter->GetDesc(&desc);

		if (desc.DedicatedVideoMemory > bestAdapterMem)
		{
			bestAdapterIndex = i;
			bestAdapterMem = desc.DedicatedVideoMemory;
		}
	}

	// Store adapter's name
	DXGI_ADAPTER_DESC desc;
	vAdapters[bestAdapterIndex]->GetDesc(&desc);
	size_t desc_len = _tcslen(desc.Description);
	m_AdapterName = new TCHAR[desc_len + 1];
	_tcscpy_s(m_AdapterName, desc_len + 1, desc.Description);

	// Create d3d11 device
	D3D_FEATURE_LEVEL featureLevel;
	HRESULT hr = D3D11CreateDevice(
		vAdapters[bestAdapterIndex],
		D3D_DRIVER_TYPE_UNKNOWN,
		0,
		createDeviceFlags,
		0, 0,
		D3D11_SDK_VERSION,
		&m_D3DDevice,
		&featureLevel,
		&m_D3DDeviceContext);

	g_RenderDev.device = m_D3DDevice.Get();
	g_RenderDev.deviceContext = m_D3DDeviceContext.Get();

	if (FAILED(hr))
	{
		TCHAR* szErrMsg;

		if (FormatMessage(
			FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
			NULL, hr, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
			(LPTSTR)&szErrMsg, 0, NULL) != 0)
		{
			TCHAR buffer[1024];
			_snwprintf_s(buffer, sizeof(buffer) / sizeof(TCHAR), 1024, L"Failed to create D3D11 device: %s", szErrMsg);

			MessageBox(0, buffer, 0, MB_ICONERROR);

			LocalFree(szErrMsg);
		}
		else
		{
			MessageBox(0, L"Failed to create D3D11 device: Unknown error.", 0, MB_ICONERROR);
		}
		return false;
	}

	if (featureLevel != D3D_FEATURE_LEVEL_11_0)
	{
		MessageBox(0, L"Direct3D Feature Level 11 unsupported.", 0, MB_ICONERROR);
		return false;
	}

	DXGI_FORMAT backbuffer_format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;

	// Check 4X MSAA quality support
	HR(m_D3DDevice->CheckMultisampleQualityLevels(
		backbuffer_format, 4, &m_MsaaQuality));
	assert(m_MsaaQuality > 0);

	DXGI_SWAP_CHAIN_DESC sd;
	sd.BufferDesc.Width = width;
	sd.BufferDesc.Height = height;
	sd.BufferDesc.RefreshRate.Numerator = 60;
	sd.BufferDesc.RefreshRate.Denominator = 1;
	sd.BufferDesc.Format = backbuffer_format;
	sd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	sd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

	if (useMsaa)
	{
		sd.SampleDesc.Count = 4;
		sd.SampleDesc.Quality = m_MsaaQuality - 1;
	}
	else
	{
		sd.SampleDesc.Count = 1;
		sd.SampleDesc.Quality = 0;
	}

	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.BufferCount = 1;
	sd.OutputWindow = hWnd;
	sd.Windowed = true;
	sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	sd.Flags = 0;

	// Create swap chain
	HR(dxgiFactory->CreateSwapChain(m_D3DDevice.Get(), &sd, &m_SwapChain));

	// Create render target view
	CreateBackBufferRTV();

	// Create depth/stencil buffer and view
	CreateDepthStencilBufferAndView();

	// Create render target for emissive color
	m_EmissiveRT = CreateRenderTarget(DXGI_FORMAT_R8G8B8A8_UNORM, true);

	// Setup viewport
	SetupViewport();

	// Create input layouts for all vertex declarations
	CreateVertexDeclarations();

	CreateShaders();

	CreateConstantBuffers();

	CreateSamplerState();

	CreateBlendState();

	CreateScreenQuad();

	CreatePostProcessorResources();
	
	// Initialize debug renderer
	DebugRenderer::Init();

	// Set all skinned mesh renderer indices (matching one-to-one with poses)
	for (unsigned int i = 0; i < s_kMaxSkinnedMeshRenderers; ++i)
	{
		m_SkinnedMeshRenderers[i].SetAnimationId( i );
	}

	g_DevMenu.AddMenuVarBool("Vsync: ", &s_bVsync);
	g_DevMenu.AddMenuVarBool("Show rendering statistics: ", &s_DrawRenderingStatistics);
	g_DevMenu.AddMenuVarBool("Debug light: ", &s_DrawDebugLightInfo);
	g_DevMenu.AddMenuVarBool("Draw render AABB: ", &s_DrawRenderComponentAabb);
	g_DevMenu.AddMenuVarBool("Show profiler info: ", &s_DrawProfilerInfo);
	g_DevMenu.AddMenuVarBool("Use pre-depth pass: ", &g_UsePreDepthPass);
	g_DevMenu.AddMenuVarBool("Debug simple pixel shader: ", &s_UseDebugSimplePixelShader);
	g_DevMenu.AddMenuVarBool("Enable post processing: ", &s_EnablePostProcessing);
	//g_DevMenu.AddMenuVar("Inner cone: ", &s_InnerCone, 0.0f, 10.0f, 0.01f);
	//g_DevMenu.AddMenuVar("Outer cone: ", &s_OuterCone, 0.0f, 10.0f, 0.01f);

	return true;
}

void Hourglass::RenderSystem::Shutdown()
{	
	delete[] m_AdapterName;

#if REPORT_LIVE_DIRECTX
	#if _DEBUG
		ComPtr<ID3D11Debug>  debug;
		m_D3DDevice.As( &debug );
		debug->ReportLiveDeviceObjects( D3D11_RLDO_DETAIL );
	#endif // END _DEBUG
#endif
}

void Hourglass::RenderSystem::Render()
{
	// Bind views to the output merger stage
	ID3D11RenderTargetView* rtvs[] =
	{
		m_BackBufferRTV.Get(),
		m_EmissiveRT.RTV.Get()
	};
	m_D3DDeviceContext->OMSetRenderTargets(2, rtvs, m_DepthStencilView.Get());

	FLOAT clearColor[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
	m_D3DDeviceContext->ClearRenderTargetView(m_BackBufferRTV.Get(), clearColor);
	m_D3DDeviceContext->ClearRenderTargetView(m_EmissiveRT.RTV.Get(), clearColor);

	m_D3DDeviceContext->ClearDepthStencilView(m_DepthStencilView.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0);

	float blendFactor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
	m_D3DDeviceContext->OMSetBlendState(g_BlendState[kBlend_Opaque].Get(), blendFactor, 0xFFFFFFFF);

	SHADER_SCENE_BUFFER cbScene;
	ZeroMemory(&cbScene, sizeof(cbScene));
	BoundingFrustum frustum;

	// Set up view projection matrix
	Camera* mainCamera = CameraSystem::GetMainCamera();
	assert(mainCamera);

	XMMATRIX viewMatrix = XMMatrixMultiply(mainCamera->GetViewMatrix(), mainCamera->GetCameraShakeMatrix());
	XMMATRIX viewProjMatrix = XMMatrixMultiply(mainCamera->GetViewMatrix(), mainCamera->GetCameraShakeMatrix());
	viewProjMatrix = XMMatrixMultiply(viewProjMatrix, mainCamera->GetProjectionMatrix());
	Vector4 cameraPos = mainCamera->GetEntity()->GetPosition();
	XMStoreFloat4x4(&cbScene.viewMatrix, viewMatrix);
	XMStoreFloat4x4(&cbScene.viewProjMatrix, viewProjMatrix);
	frustum = mainCamera->GetViewFrustum();

	float cw = (float)m_ClientWidth;
	float ch = (float)m_ClientHeight;
	cbScene.screenSize = Vector4(cw, ch, 1 / cw, 1 / ch);

#if 0
	DirectX::XMMATRIX viewMatrix = DirectX::XMMatrixLookAtLH(DirectX::XMVectorSet(0, 8, -8, 1), DirectX::XMVectorSet(0, 0, 0, 1), DirectX::XMVectorSet(0, 1, 0, 1));
	DirectX::XMMATRIX projMatrix = DirectX::XMMatrixPerspectiveFovLH(DirectX::XMConvertToRadians(75.0f), GetClientAspect(), 0.1f, 100.0f);
	DirectX::XMStoreFloat4x4(&buffer.viewProjMatrix, DirectX::XMMatrixMultiply(viewMatrix, projMatrix));
#endif
	SetShaderSceneConstBuffer(cbScene);
	BEGIN_PROFILER_BLOCK("Build Light List");
	// Build visible light list
	std::vector<Light*> visibleLightList = BuildVisibleLightList(frustum);
	END_PROFILER_BLOCK("Build Light List");

	BEGIN_PROFILER_BLOCK("Debug Light Info");
	// Draw light debug info
	if (s_DrawDebugLightInfo)
	{
		for (int i = 0; i < visibleLightList.size(); i++)
		{
			visibleLightList[i]->DrawDebugLightInfo();
		}
	}
	END_PROFILER_BLOCK("Debug Light Info");

	BEGIN_PROFILER_BLOCK("Build Render Queue");
	for (unsigned int i = 0; i < s_kMaxPrimitiveRenderers; ++i)
	{
		PrimitiveRenderer& pr = m_PrimitiveRenderers[i];

		if (!pr.IsAlive() || !pr.IsEnabled())
			continue;

		// Frustum culling
		if (!frustum.Contains(pr.GetRenderAabb().GetDirectXBoundingBox()))
			continue;

		pr.AddToRenderQueue(m_RenderQueue, visibleLightList);
	}
	
	for (unsigned int i = 0; i < s_kMaxMeshRenderers; ++i)
	{
		MeshRenderer& mr = m_MeshRenderers[i];

		if (!mr.IsAlive() || !mr.IsEnabled())
			continue;

		// Frustum culling
		if (!frustum.Contains(mr.GetRenderAabb().GetDirectXBoundingBox()))
			continue;

		if (s_DrawRenderComponentAabb)
			DebugRenderer::DrawAabb(mr.GetRenderAabb(), Color(0.0f, 1.0f, 1.0f));

		mr.AddToRenderQueue(m_RenderQueue, visibleLightList);
	}

	for (unsigned int i = 0; i < s_kMaxSkinnedMeshRenderers; ++i)
	{
		if (m_SkinnedMeshRenderers[i].IsAlive() && m_SkinnedMeshRenderers[i].IsEnabled())
		{
			if (s_DrawRenderComponentAabb)
				DebugRenderer::DrawAabb(m_SkinnedMeshRenderers[i].GetRenderAabb(), Color(0.0f, 1.0f, 1.0f));

			m_SkinnedMeshRenderers[i].AddToRenderQueue( m_RenderQueue, visibleLightList);
		}
	}
	END_PROFILER_BLOCK("Build Render Queue");

	BEGIN_PROFILER_BLOCK("Present Render Queue");
	m_DrawCallCount = 0;

	// Render pre-depth pass
	if (g_UsePreDepthPass)
	{
		// Enable depth writing for pre-depth pass
		m_D3DDeviceContext->OMSetDepthStencilState(m_DepthStencilState[kDepthState_WriteAndTest].Get(), 0);
		
		RenderPreDepthPass();
	}

	// Render objects in render queue
	for (int p = 0; p < kRenderPassCount; p++)
	{
		// Disable depth writing for opaque pass when using pre-depth pass
		if (g_UsePreDepthPass && p == kRenderPass_Opaque)
			m_D3DDeviceContext->OMSetDepthStencilState(m_DepthStencilState[kDepthState_NoWrite].Get(), 0);
		else
			m_D3DDeviceContext->OMSetDepthStencilState(m_DepthStencilState[kDepthState_WriteAndTest].Get(), 0);

		RenderPass& pass = m_RenderQueue.m_RenderPasses[p];

		// TODO: Set up passes
		ID3D11ShaderResourceView** cube = g_TextureManager.GetEnvirnmental(0)->ShaderResourceView.GetAddressOf();
		m_D3DDeviceContext->PSSetShaderResources(7, 1, cube);
		for (int t = 0; t < pass.m_RenderTechniques.size(); t++)
		{
			RenderTechnique& tech = pass.m_RenderTechniques[t];

			// TODO: Set up techniques
			g_RenderDev.deviceContext->VSSetShader(tech.m_TechniqueData.m_VertexShader, nullptr, 0);

			if (s_UseDebugSimplePixelShader)
				g_RenderDev.deviceContext->PSSetShader(g_RenderShaders[kRenderShader_DebugSimpleColor].PixelShader.Get(), nullptr, 0);
			else
				g_RenderDev.deviceContext->PSSetShader(tech.m_TechniqueData.m_PixelShader, nullptr, 0);

			for (int m = 0; m < tech.m_Materials.size(); m++)
			{
				RenderMaterial& material = tech.m_Materials[m];

				m_D3DDeviceContext->OMSetBlendState(g_BlendState[material.m_MaterialData.m_BlendMode].Get(), blendFactor, 0xFFFFFFFF);

				// TODO: Set up materials
				SHADER_OBJECT_BUFFER buffer;
				ZeroMemory(&buffer, sizeof(buffer));

				if (material.m_MaterialData.m_UseUniformColor)
					buffer.uniformColor = material.m_MaterialData.m_UniformColor;
				else
					buffer.uniformColor = Color(1.0f, 1.0f, 1.0f);

				g_RenderDev.deviceContext->PSSetSamplers(0, 1, g_SamplerState[material.m_MaterialData.m_SamplerState].GetAddressOf());

				// Bind all shader resources to pixel shader
				g_RenderDev.deviceContext->PSSetShaderResources(0, 7, &material.m_MaterialData.m_ShaderResourceView[0]);

				bool useNormapMap = (material.m_MaterialData.m_ShaderResourceView[1] != nullptr);
				bool useSpecularMap = (material.m_MaterialData.m_ShaderResourceView[2] != nullptr);
				bool useEmissiveMap = (material.m_MaterialData.m_ShaderResourceView[3] != nullptr);

				for (int s = 0; s < material.m_RenderShapes.size(); s++)
				{
					RenderShape shape = material.m_RenderShapes[s];

					// Set up bone matrices for skinned mesh
					if (shape.useAnimation)
					{
						SetShaderPoseConstBuffer( m_Poses[shape.animationId] );
					}

					// Set up lighting
					SHADER_LIGHT_BUFFER lightBuffer;
					ZeroMemory(&lightBuffer, sizeof(SHADER_LIGHT_BUFFER));
					lightBuffer.lightCount = shape.lightCount;
					lightBuffer.useNormalMap = useNormapMap;
					lightBuffer.useSpecularMap = useSpecularMap;
					lightBuffer.useEmissiveMap = useEmissiveMap;
					lightBuffer.cameraPos = cameraPos;
					lightBuffer.time = hg::g_Time.Elapsed();

					for (UINT i = 0; i < shape.lightCount; i++)
					{
						// Store light position
						Vector4 pos_r = Vector4(shape.lights[i]->GetPosition());

						// Store light radius in w component
						pos_r.w = shape.lights[i]->GetRadius();

						lightBuffer.lights[i].PositionRadius = pos_r;
						lightBuffer.lights[i].Color = shape.lights[i]->GetColor();
						lightBuffer.lights[i].Color.w *= shape.lights[i]->GetIntensity();

						lightBuffer.lights[i].Type = shape.lights[i]->GetLightType();

						if (shape.lights[i]->GetLightType() == kLightType_Spotlight)
						{
							lightBuffer.lights[i].Direction = shape.lights[i]->GetDirection();
							lightBuffer.lights[i].ConeRatio = Vector4(shape.lights[i]->GetInnerConeRatio(), shape.lights[i]->GetOuterConeRatio(), 0.0f, 0.0f);
						}
					}
					SetShaderLightConstBuffer(lightBuffer);

					// Set up world matrix
					if (shape.useMatrix)
						XMStoreFloat4x4(&buffer.worldMatrix, shape.matrix);
					else
						XMStoreFloat4x4(&buffer.worldMatrix, shape.transform->GetMatrix());

					SetShaderObjectConstBuffer(buffer);

					//if (shape.submesh)
						//shape.buffer->DrawIndexed(shape.submesh->indexCount, shape.submesh->startIndexOffset, shape.submesh->baseVertexOffset);
					//else
						shape.buffer->Draw();

					m_DrawCallCount++;
				}
			}
		}
	}

	m_RenderQueue.Clear();
	END_PROFILER_BLOCK("Present Render Queue");

	BEGIN_PROFILER_BLOCK("Rendering Particle");
	g_particleSystemGPU.Render();
	END_PROFILER_BLOCK("Rendering Particle");

	SetShaderSceneConstBuffer(cbScene);
	BEGIN_PROFILER_BLOCK("Post processing");

	SHADER_OBJECT_BUFFER ppShaderObjectBuffer;
	ZeroMemory(&ppShaderObjectBuffer, sizeof(ppShaderObjectBuffer));

	if (s_EnablePostProcessing)
	{
		// Compute shader post processing

		// Unbind emissive render target before using it as post processing texture
		m_D3DDeviceContext->OMSetRenderTargets(1, m_BackBufferRTV.GetAddressOf(), m_DepthStencilView.Get());
		m_D3DDeviceContext->CSSetConstantBuffers(0, 1, m_ConstBufferPerScene.GetAddressOf());
		m_D3DDeviceContext->CSSetShader(g_ComputeShaders[kComputeShader_Blur].Get(), nullptr, 0);

		if (m_MsaaEnabled)
		{
			D3D11_TEXTURE2D_DESC desc;

			m_EmissiveRT.Buffer->GetDesc(&desc);

			m_D3DDeviceContext->ResolveSubresource(
				m_EmissiveRT.ResolvedBuffer.Get(), D3D11CalcSubresource(0, 0, 1),
				m_EmissiveRT.Buffer.Get(), D3D11CalcSubresource(0, 0, 1),
				desc.Format
			);

			m_D3DDeviceContext->GenerateMips(m_EmissiveRT.ResolvedSRV.Get());
			m_D3DDeviceContext->CSSetShaderResources(0, 1, m_EmissiveRT.ResolvedSRV.GetAddressOf());
		}
		else
		{
			m_D3DDeviceContext->GenerateMips(m_EmissiveRT.SRV.Get());
			m_D3DDeviceContext->CSSetShaderResources(0, 1, m_EmissiveRT.SRV.GetAddressOf());
		}

		m_D3DDeviceContext->CSSetUnorderedAccessViews(0, 1, m_PostProcessorOutputUAV.GetAddressOf(), nullptr);
		m_D3DDeviceContext->CSSetSamplers(0, 1, g_SamplerState[kSamplerState_Bilinear_Clamp].GetAddressOf());

		UINT cs_x = UINT(ceilf((float)m_ClientWidth / 16));
		UINT cs_y = UINT(ceilf((float)m_ClientHeight / 16));

		m_D3DDeviceContext->Dispatch(cs_x, cs_y, 1);
		m_D3DDeviceContext->CSSetShader(nullptr, nullptr, 0);
		ID3D11ShaderResourceView* nullSRVs[] = { nullptr };
		m_D3DDeviceContext->CSSetShaderResources(0, 1, nullSRVs);
		ID3D11UnorderedAccessView* nullUAVs[] = { nullptr };
		m_D3DDeviceContext->CSSetUnorderedAccessViews(0, 1, nullUAVs, nullptr);

		// Render post processor
		//m_D3DDeviceContext->ClearRenderTargetView(m_BackBufferRTV.Get(), clearColor);
		m_D3DDeviceContext->OMSetDepthStencilState(m_DepthStencilState[kDepthState_NoWrite].Get(), 0);

		ppShaderObjectBuffer.uniformColor = Vector4(1, 1, 1, 1);
		SetShaderObjectConstBuffer(ppShaderObjectBuffer);

		g_RenderDev.deviceContext->VSSetShader(g_RenderShaders[kRenderShader_PostProcessor].VertexShader.Get(), nullptr, 0);
		g_RenderDev.deviceContext->PSSetShader(g_RenderShaders[kRenderShader_PostProcessor].PixelShader.Get(), nullptr, 0);
		g_RenderDev.deviceContext->PSSetShaderResources(0, 1, m_PostProcessorOutputSRV.GetAddressOf());
		m_D3DDeviceContext->OMSetBlendState(g_BlendState[kBlend_Additive].Get(), blendFactor, 0xFFFFFFFF);

		m_ScreenQuad.Draw();
		g_RenderDev.deviceContext->PSSetShaderResources(0, 1, nullSRVs);
	}

	// Render screen overlay texture
	if (g_ScreenOverlay)
	{
		ppShaderObjectBuffer.uniformColor = g_ScreenOverlayColor;
		SetShaderObjectConstBuffer(ppShaderObjectBuffer);

		g_RenderDev.deviceContext->VSSetShader(g_RenderShaders[kRenderShader_PostProcessor].VertexShader.Get(), nullptr, 0);
		g_RenderDev.deviceContext->PSSetShader(g_RenderShaders[kRenderShader_PostProcessor].PixelShader.Get(), nullptr, 0);
		g_RenderDev.deviceContext->PSSetShaderResources(0, 1, g_ScreenOverlay->ShaderResourceView.GetAddressOf());
		m_D3DDeviceContext->OMSetBlendState(g_BlendState[kBlend_AlphaBlend].Get(), blendFactor, 0xFFFFFFFF);
		m_ScreenQuad.Draw();
	}
	END_PROFILER_BLOCK("Post processing");

	BEGIN_PROFILER_BLOCK("Present Debug Lines");

	m_D3DDeviceContext->OMSetDepthStencilState(m_DepthStencilState[kDepthState_WriteAndTest].Get(), 0);
	m_D3DDeviceContext->OMSetBlendState(g_BlendState[kBlend_Opaque].Get(), blendFactor, 0xFFFFFFFF);
	g_RenderDev.deviceContext->VSSetShader(g_RenderShaders[kRenderShader_Color].VertexShader.Get(), nullptr, 0);
	g_RenderDev.deviceContext->PSSetShader(g_RenderShaders[kRenderShader_Color].PixelShader.Get(), nullptr, 0);

	// Set up identity world matrix for debug renderer
	{
		SHADER_OBJECT_BUFFER buffer;
		ZeroMemory(&buffer, sizeof(buffer));

		// Set up world matrix
		DirectX::XMStoreFloat4x4(&buffer.worldMatrix, DirectX::XMMatrixIdentity());

		SetShaderObjectConstBuffer(buffer);
	}

	DebugRenderer::Render();
	DebugRenderer::Reset();
	END_PROFILER_BLOCK("Present Debug Lines");

	BEGIN_PROFILER_BLOCK("Present Dev Menu");
	g_DevMenu.Draw(80, 2);
	END_PROFILER_BLOCK("Present Dev Menu");

	BEGIN_PROFILER_BLOCK("Present Statistics");
	static float fps = 0.0f;

	static int frameCnt = 0;
	static float timeElapsed = 0.0f;

	frameCnt++;

	// Compute averages over one second period.
	if ((g_Time.Elapsed() - timeElapsed) >= 1.0f)
	{
		fps = (float)frameCnt; // fps = frameCnt / 1

		// Reset for next average.
		frameCnt = 0;
		timeElapsed += 1.0f;
	}

	if (s_DrawRenderingStatistics)
	{
		char buf[1024];
		sprintf_s(buf, "Draw calls: %d\nVisible lights: %d\nFPS: %d", m_DrawCallCount, (int)visibleLightList.size(), (int)fps);
		DevTextRenderer::DrawText(buf, 0, 0);
	}
	END_PROFILER_BLOCK("Present Statistics");

	BEGIN_PROFILER_BLOCK("Print Profiler Info");
	if (s_DrawProfilerInfo)
	{
		g_Profiler.PrintResult();
	}
	END_PROFILER_BLOCK("Print Profiler Info");

	BEGIN_PROFILER_BLOCK("Render Dev Text");
	DevTextRenderer::Render();
	DevTextRenderer::Clear();
	END_PROFILER_BLOCK("Render Dev Text");

	
	BEGIN_PROFILER_BLOCK("Present Swap Chain");
	HR(m_SwapChain->Present(s_bVsync ? 1 : 0, 0));
	END_PROFILER_BLOCK("Present Swap Chain");
}

std::vector<Hourglass::Light*> Hourglass::RenderSystem::BuildVisibleLightList(const BoundingFrustum& frustum)
{
	std::vector<Light*> visibleLightList;

	for (unsigned int i = 0; i < s_kMaxLights; i++)
	{
		// Make sure light is alive and enabled
		Light& light = m_Lights[i];
		if (!light.IsEnabled() || !light.IsAlive())
			continue;

		// Update light bounding shapes
		light.CacheBoundingShapes();

		// Make sure light is inside view frustum
		if (!light.TestContainedByFrustum(frustum))
			continue;

		// Add visible light to the list
		visibleLightList.push_back(&m_Lights[i]);
	}

	return visibleLightList;
}

void Hourglass::RenderSystem::RenderPreDepthPass()
{
	g_RenderDev.deviceContext->VSSetShader(g_RenderShaders[kRenderShader_Depth].VertexShader.Get(), nullptr, 0);
	g_RenderDev.deviceContext->PSSetShader(nullptr, nullptr, 0);
	g_RenderDev.deviceContext->PSSetSamplers(0, 1, g_SamplerState[kSamplerState_Point].GetAddressOf());

	SHADER_OBJECT_BUFFER buffer;
	ZeroMemory(&buffer, sizeof(buffer));

	for (int i = 0; i < m_RenderQueue.m_DepthPassShapes.size(); i++)
	{
		RenderShape shape = m_RenderQueue.m_DepthPassShapes[i];

		// Set up world matrix
		if (shape.useMatrix)
			XMStoreFloat4x4(&buffer.worldMatrix, shape.matrix);
		else
			XMStoreFloat4x4(&buffer.worldMatrix, shape.transform->GetMatrix());

		SetShaderObjectConstBuffer(buffer);

		if (shape.submesh)
			shape.buffer->DrawIndexed(shape.submesh->indexCount, shape.submesh->startIndexOffset, shape.submesh->baseVertexOffset);
		else
			shape.buffer->Draw();

		m_DrawCallCount++;
	}
}

float Hourglass::RenderSystem::GetClientAspect() const
{
	return (float)m_ClientWidth / (float)m_ClientHeight;
}

void Hourglass::RenderSystem::OnResize(int width, int height)
{
	if (!width || !height)
		return;

	m_ClientWidth = width;
	m_ClientHeight = height;

	if (!m_SwapChain)
		return;

	ID3D11RenderTargetView* nullRTVs[] = { nullptr };
	m_D3DDeviceContext->OMSetRenderTargets(1, nullRTVs, nullptr);

	// Resources should be released before resizing swap chain
	m_DepthStencilBuffer = nullptr;
	m_DepthStencilView = nullptr;
	m_BackBufferRTV = nullptr;

	m_EmissiveRT.Buffer = nullptr;
	m_EmissiveRT.RTV = nullptr;
	m_EmissiveRT.SRV = nullptr;
	m_EmissiveRT.ResolvedBuffer = nullptr;
	m_EmissiveRT.ResolvedSRV = nullptr;

	ResizeSwapChain(width, height);

	CreateBackBufferRTV();
	CreateDepthStencilBufferAndView();
	m_D3DDeviceContext->OMSetRenderTargets(1, m_BackBufferRTV.GetAddressOf(), m_DepthStencilView.Get());

	m_EmissiveRT = CreateRenderTarget(DXGI_FORMAT_R8G8B8A8_UNORM, true);

	m_PostProcessorOutputBuffer = nullptr;
	m_PostProcessorOutputUAV = nullptr;
	m_PostProcessorOutputSRV = nullptr;

	CreatePostProcessorResources();

	SetupViewport();
}

void Hourglass::RenderSystem::TogglePostProcess()
{
	s_EnablePostProcessing = !s_EnablePostProcessing;
}

void Hourglass::RenderSystem::ToggleVsync()
{
	s_bVsync = !s_bVsync;
}

void Hourglass::RenderSystem::SetFlags(bool enable)
{
	s_bVsync = !enable;
	s_EnablePostProcessing = enable;
}

void Hourglass::RenderSystem::CreateBackBufferRTV()
{
	ComPtr<ID3D11Texture2D> backBuffer;
	m_SwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D),
		reinterpret_cast<void**>(backBuffer.GetAddressOf()));
	m_D3DDevice->CreateRenderTargetView(backBuffer.Get(), 0, &m_BackBufferRTV);
}

void Hourglass::RenderSystem::CreateDepthStencilBufferAndView()
{
	D3D11_TEXTURE2D_DESC depthStencilDesc;
	depthStencilDesc.Width = m_ClientWidth;
	depthStencilDesc.Height = m_ClientHeight;
	depthStencilDesc.MipLevels = 1;
	depthStencilDesc.ArraySize = 1;
	depthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;

	if (m_MsaaEnabled)
	{
		depthStencilDesc.SampleDesc.Count = 4;
		depthStencilDesc.SampleDesc.Quality = m_MsaaQuality - 1;
	}
	else
	{
		depthStencilDesc.SampleDesc.Count = 1;
		depthStencilDesc.SampleDesc.Quality = 0;
	}

	depthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
	depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthStencilDesc.CPUAccessFlags = 0;
	depthStencilDesc.MiscFlags = 0;

	HR(m_D3DDevice->CreateTexture2D(&depthStencilDesc, 0, &m_DepthStencilBuffer));
	HR(m_D3DDevice->CreateDepthStencilView(m_DepthStencilBuffer.Get(), 0, &m_DepthStencilView));

	D3D11_DEPTH_STENCIL_DESC dsDesc;

	// Depth test parameters
	dsDesc.DepthEnable = true;
	dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	dsDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;

	// Stencil test parameters
	dsDesc.StencilEnable = false;
	dsDesc.StencilReadMask = 0xFF;
	dsDesc.StencilWriteMask = 0xFF;

	// Stencil operations if pixel is front-facing
	dsDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	dsDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	dsDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	dsDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	// Stencil operations if pixel is back-facing
	dsDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	dsDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	dsDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	dsDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	// Create depth stencil state
	m_D3DDevice->CreateDepthStencilState(&dsDesc, &m_DepthStencilState[kDepthState_WriteAndTest]);

	dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
	m_D3DDevice->CreateDepthStencilState(&dsDesc, &m_DepthStencilState[kDepthState_NoWrite]);

	m_D3DDeviceContext->OMSetDepthStencilState(m_DepthStencilState[kDepthState_WriteAndTest].Get(), 0);
}

void Hourglass::RenderSystem::SetupViewport()
{
	D3D11_VIEWPORT vp;
	vp.TopLeftX = 0.0f;
	vp.TopLeftY = 0.0f;
	vp.Width = static_cast<float>(m_ClientWidth);
	vp.Height = static_cast<float>(m_ClientHeight);
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;

	m_D3DDeviceContext->RSSetViewports(1, &vp);
}

void Hourglass::RenderSystem::CreateVertexDeclarations()
{
	// Colored vertex
	D3D11_INPUT_ELEMENT_DESC colorVertexDesc[] =
	{
		{ "POSITION",	0, DXGI_FORMAT_R32G32B32A32_FLOAT,	0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR",		0, DXGI_FORMAT_R32G32B32A32_FLOAT,	0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	m_D3DDevice->CreateInputLayout(colorVertexDesc, 2, ColorVertexSignature, sizeof(ColorVertexSignature), &g_InputLayouts[kVertexDecl_Color]);

	// Font vertex
	D3D11_INPUT_ELEMENT_DESC fontVertexDesc[] =
	{
		{ "POSITION",	0, DXGI_FORMAT_R32G32B32_FLOAT,		0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR",		0, DXGI_FORMAT_R32G32B32A32_FLOAT,	0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR",		1, DXGI_FORMAT_R32G32B32A32_FLOAT,	0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD",	0, DXGI_FORMAT_R32G32_FLOAT,		0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	m_D3DDevice->CreateInputLayout(fontVertexDesc, 4, FontVertexSignature, sizeof(FontVertexSignature), &g_InputLayouts[kVertexDecl_Font]);

	// Mesh vertex
	D3D11_INPUT_ELEMENT_DESC meshVertexDesc[] =
	{
		{ "POSITION",	0, DXGI_FORMAT_R32G32B32_FLOAT,		0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD",	0, DXGI_FORMAT_R32G32_FLOAT,		0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL",		0, DXGI_FORMAT_R32G32B32_FLOAT,		0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TANGENT",	0, DXGI_FORMAT_R32G32B32A32_FLOAT,	0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	m_D3DDevice->CreateInputLayout(meshVertexDesc, 4, MeshVertexSignature, sizeof(MeshVertexSignature), &g_InputLayouts[kVertexDecl_PosUV0NormTan]);

	// Skinned mesh vertex
	D3D11_INPUT_ELEMENT_DESC skMeshVertexDesc[] =
	{
		{ "POSITION",	0, DXGI_FORMAT_R32G32B32_FLOAT,		0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD",	0, DXGI_FORMAT_R32G32_FLOAT,		0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL",		0, DXGI_FORMAT_R32G32B32_FLOAT,		0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TANGENT",	0, DXGI_FORMAT_R32G32B32A32_FLOAT,	0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "INFLUENCES",	0, DXGI_FORMAT_R32G32B32A32_SINT,	0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "WEIGHTS",	0, DXGI_FORMAT_R32G32B32A32_FLOAT,	0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	m_D3DDevice->CreateInputLayout( skMeshVertexDesc, 6, SkinnedMeshVertexSignature, sizeof( SkinnedMeshVertexSignature ), &g_InputLayouts[kVertexDecl_PosUV0NormTanSkin] );
}

void Hourglass::RenderSystem::CreateShaders()
{
	g_RenderShaders[kRenderShader_Color].VertexShader						= CompileVertexShader ("HLSL/Color_VS.hlsl");
	g_RenderShaders[kRenderShader_Color].PixelShader						= CompilePixelShader  ("HLSL/Color_PS.hlsl");
	g_RenderShaders[kRenderShader_Font].VertexShader						= CompileVertexShader ("HLSL/Font_VS.hlsl");
	g_RenderShaders[kRenderShader_Font].PixelShader							= CompilePixelShader  ("HLSL/Font_PS.hlsl");
	g_RenderShaders[kRenderShader_Lighting].VertexShader					= CompileVertexShader ("HLSL/Lighting_VS.hlsl");
	g_RenderShaders[kRenderShader_Lighting].PixelShader						= CompilePixelShader  ("HLSL/Lighting_PS.hlsl");
	g_RenderShaders[kRenderShader_LightingSkinned].VertexShader				= CompileVertexShader ("HLSL/LightingSkinned_VS.hlsl");
	g_RenderShaders[kRenderShader_LightingSkinned_LightWeight].VertexShader = CompileVertexShader ("HLSL/LightingSkinned_LightWeight_VS.hlsl");
	g_RenderShaders[kRenderShader_PbrLighting].PixelShader					= CompilePixelShader  ("HLSL/PbrLighting_PS.hlsl");
	g_RenderShaders[kRenderShader_PbrFlow].PixelShader						= CompilePixelShader  ("HLSL/PbrFlow_PS.hlsl");
	g_RenderShaders[kRenderShader_Barrier].PixelShader						= CompilePixelShader  ("HLSL/Barrier_PS.hlsl" );
	g_RenderShaders[kRenderShader_Depth].VertexShader						= CompileVertexShader ("HLSL/Depth_VS.hlsl");
	g_RenderShaders[kRenderShader_DebugSimpleColor].PixelShader				= CompilePixelShader  ("HLSL/DebugSimpleColor_PS.hlsl");
	g_RenderShaders[kRenderShader_PostProcessor].VertexShader				= CompileVertexShader ("HLSL/PostProcessor_VS.hlsl");
	g_RenderShaders[kRenderShader_PostProcessor].PixelShader				= CompilePixelShader  ("HLSL/PostProcessor_PS.hlsl");

	g_ComputeShaders[kComputeShader_Blur] = CompileComputeShader("HLSL/Blur_CS.hlsl");
}

void Hourglass::RenderSystem::CreateConstantBuffers()
{
	D3D11_BUFFER_DESC cbDesc;
	ZeroMemory(&cbDesc, sizeof(cbDesc));
	cbDesc.ByteWidth = sizeof(SHADER_SCENE_BUFFER);
	cbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cbDesc.Usage = D3D11_USAGE_DYNAMIC;

	{
		m_D3DDevice->CreateBuffer(&cbDesc, NULL, &m_ConstBufferPerScene);

		// Initialize buffer values with zero
		SHADER_SCENE_BUFFER buffer;
		ZeroMemory(&buffer, sizeof(buffer));

		D3D11_MAPPED_SUBRESOURCE subres;
		m_D3DDeviceContext->Map(m_ConstBufferPerScene.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &subres);
		memcpy(subres.pData, &buffer, sizeof(buffer));
		m_D3DDeviceContext->Unmap(m_ConstBufferPerScene.Get(), 0);
	}

	cbDesc.ByteWidth = sizeof(SHADER_OBJECT_BUFFER);

	{
		m_D3DDevice->CreateBuffer(&cbDesc, NULL, &m_ConstBufferPerObject);

		// Initialize buffer values with zero
		SHADER_OBJECT_BUFFER buffer;
		ZeroMemory(&buffer, sizeof(buffer));

		D3D11_MAPPED_SUBRESOURCE subres;
		m_D3DDeviceContext->Map(m_ConstBufferPerObject.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &subres);
		memcpy(subres.pData, &buffer, sizeof(buffer));
		m_D3DDeviceContext->Unmap(m_ConstBufferPerObject.Get(), 0);
	}

	cbDesc.ByteWidth = sizeof( POSE_BUFFER );

	{
		m_D3DDevice->CreateBuffer( &cbDesc, NULL, &m_ConstBufferPose );

		// Initialize buffer values with zero
		POSE_BUFFER buffer;
		ZeroMemory( &buffer, sizeof( buffer ) );

		D3D11_MAPPED_SUBRESOURCE subres;
		m_D3DDeviceContext->Map( m_ConstBufferPose.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &subres );
		memcpy( subres.pData, &buffer, sizeof( buffer ) );
		m_D3DDeviceContext->Unmap( m_ConstBufferPose.Get(), 0 );
	}

	cbDesc.ByteWidth = sizeof(SHADER_LIGHT_BUFFER);

	{
		m_D3DDevice->CreateBuffer(&cbDesc, NULL, &m_ConstBufferLighting);

		// Initialize buffer values with zero
		SHADER_LIGHT_BUFFER buffer;
		ZeroMemory(&buffer, sizeof(buffer));

		D3D11_MAPPED_SUBRESOURCE subres;
		m_D3DDeviceContext->Map(m_ConstBufferLighting.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &subres);
		memcpy(subres.pData, &buffer, sizeof(buffer));
		m_D3DDeviceContext->Unmap(m_ConstBufferLighting.Get(), 0);
	}

}

void Hourglass::RenderSystem::CreateSamplerState()
{
	// Create sampler states
	D3D11_SAMPLER_DESC samplerDesc;
	ZeroMemory(&samplerDesc, sizeof(samplerDesc));
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_POINT_MIP_LINEAR;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.MaxAnisotropy = 1;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

	m_D3DDevice->CreateSamplerState(&samplerDesc, &g_SamplerState[kSamplerState_Point]);

	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_LINEAR_MIP_POINT;
	m_D3DDevice->CreateSamplerState(&samplerDesc, &g_SamplerState[kSamplerState_Bilinear]);

	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	m_D3DDevice->CreateSamplerState(&samplerDesc, &g_SamplerState[kSamplerState_Trilinear]);

	samplerDesc.Filter = D3D11_FILTER_ANISOTROPIC;
	samplerDesc.MaxAnisotropy = D3D11_REQ_MAXANISOTROPY;
	m_D3DDevice->CreateSamplerState(&samplerDesc, &g_SamplerState[kSamplerState_Anisotropic]);


	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_LINEAR_MIP_POINT;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	m_D3DDevice->CreateSamplerState(&samplerDesc, &g_SamplerState[kSamplerState_Bilinear_Clamp]);
}

void Hourglass::RenderSystem::CreateBlendState()
{

	// Create blend states
	D3D11_BLEND_DESC blendDesc;
	ZeroMemory(&blendDesc, sizeof(blendDesc));
	blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	m_D3DDevice->CreateBlendState(&blendDesc, &g_BlendState[kBlend_Opaque]);

#if _DEBUG
	{
		char blendStateName[] = "Opaque";
		g_BlendState[kBlend_Opaque]->SetPrivateData(WKPDID_D3DDebugObjectName, sizeof(blendStateName) - 1, blendStateName);
	}
#endif

	blendDesc.RenderTarget[0].BlendEnable = true;
	blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	m_D3DDevice->CreateBlendState(&blendDesc, &g_BlendState[kBlend_AlphaBlend]);

#if _DEBUG
	{
		char blendStateName[] = "Alpha Blending";
		g_BlendState[kBlend_AlphaBlend]->SetPrivateData(WKPDID_D3DDebugObjectName, sizeof(blendStateName) - 1, blendStateName);
	}
#endif

	blendDesc.AlphaToCoverageEnable = true;
	m_D3DDevice->CreateBlendState(&blendDesc, &g_BlendState[kBlend_AlphaToCoverage]);

#if _DEBUG
	{
		char blendStateName[] = "Alpha To Coverage Blending";
		g_BlendState[kBlend_AlphaToCoverage]->SetPrivateData(WKPDID_D3DDebugObjectName, sizeof(blendStateName) - 1, blendStateName);
	}
#endif

	blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
	blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
	blendDesc.AlphaToCoverageEnable = false;
	m_D3DDevice->CreateBlendState(&blendDesc, &g_BlendState[kBlend_Additive]);

#if _DEBUG
	{
		char blendStateName[] = "Additive Blending";
		g_BlendState[kBlend_Additive]->SetPrivateData(WKPDID_D3DDebugObjectName, sizeof(blendStateName) - 1, blendStateName);
	}
#endif

}

void Hourglass::RenderSystem::CreateScreenQuad()
{
	ColorVertex quadVerts[] =
	{
		{ Vector4(-1.0f, -1.0f, 1.0f, 1.0f), Color(0, 0, 0, 0) },
		{ Vector4(-1.0f,  1.0f, 1.0f, 1.0f), Color(0, 0, 0, 0) },
		{ Vector4(1.0f,  1.0f, 1.0f, 1.0f),  Color(0, 0, 0, 0) },

		{ Vector4(-1.0f, -1.0f, 1.0f, 1.0f), Color(0, 0, 0, 0) },
		{ Vector4(1.0f,  1.0f, 1.0f, 1.0f),  Color(0, 0, 0, 0) },
		{ Vector4(1.0f, -1.0f, 1.0f, 1.0f),  Color(0, 0, 0, 0) },
	};

	m_ScreenQuad.CreateVertexBuffer(&quadVerts, sizeof(ColorVertex), 6,
		D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST, g_InputLayouts[kVertexDecl_Color].Get());
}

void Hourglass::RenderSystem::CreatePostProcessorResources()
{
	D3D11_TEXTURE2D_DESC textureDesc;

	ZeroMemory(&textureDesc, sizeof(D3D11_TEXTURE2D_DESC));
	textureDesc.Width = m_ClientWidth;
	textureDesc.Height = m_ClientHeight;
	textureDesc.MipLevels = 1;
	textureDesc.ArraySize = 1;
	textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags = D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE;
	textureDesc.CPUAccessFlags = 0;
	textureDesc.MiscFlags = 0;
	m_D3DDevice->CreateTexture2D(&textureDesc, NULL, m_PostProcessorOutputBuffer.GetAddressOf());

	D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc;

	// Create unordered access view
	uavDesc.Format = textureDesc.Format;
	uavDesc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2D;
	uavDesc.Texture2D.MipSlice = 0;
	m_D3DDevice->CreateUnorderedAccessView(m_PostProcessorOutputBuffer.Get(), &uavDesc, m_PostProcessorOutputUAV.GetAddressOf());

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;

	// Create shader resource view
	srvDesc.Format = textureDesc.Format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.MipLevels = 1;
	m_D3DDevice->CreateShaderResourceView(m_PostProcessorOutputBuffer.Get(), &srvDesc, m_PostProcessorOutputSRV.GetAddressOf());
}

void Hourglass::RenderSystem::ResizeSwapChain(int width, int height)
{
	HR(m_SwapChain->ResizeBuffers(0, width, height, DXGI_FORMAT_UNKNOWN, 0));
}

void Hourglass::RenderSystem::SetShaderPoseConstBuffer( const Hourglass::Pose& buffer )
{
	D3D11_MAPPED_SUBRESOURCE subres;
	m_D3DDeviceContext->Map( m_ConstBufferPose.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &subres );
	memcpy( subres.pData, &buffer, sizeof( buffer ) );
	m_D3DDeviceContext->Unmap( m_ConstBufferPose.Get(), 0 );

	m_D3DDeviceContext->VSSetConstantBuffers( 2, 1, m_ConstBufferPose.GetAddressOf() );
}

void Hourglass::RenderSystem::SetShaderObjectConstBuffer(const SHADER_OBJECT_BUFFER& buffer)
{
	D3D11_MAPPED_SUBRESOURCE subres;
	m_D3DDeviceContext->Map(m_ConstBufferPerObject.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &subres);
	memcpy(subres.pData, &buffer, sizeof(buffer));
	m_D3DDeviceContext->Unmap(m_ConstBufferPerObject.Get(), 0);

	m_D3DDeviceContext->VSSetConstantBuffers(1, 1, m_ConstBufferPerObject.GetAddressOf());
}

void Hourglass::RenderSystem::SetShaderSceneConstBuffer(const SHADER_SCENE_BUFFER& buffer)
{
	D3D11_MAPPED_SUBRESOURCE subres;
	m_D3DDeviceContext->Map(m_ConstBufferPerScene.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &subres);
	memcpy(subres.pData, &buffer, sizeof(buffer));
	m_D3DDeviceContext->Unmap(m_ConstBufferPerScene.Get(), 0);

	m_D3DDeviceContext->VSSetConstantBuffers(0, 1, m_ConstBufferPerScene.GetAddressOf());
}

void Hourglass::RenderSystem::SetShaderLightConstBuffer(const SHADER_LIGHT_BUFFER& buffer)
{
	D3D11_MAPPED_SUBRESOURCE subres;
	m_D3DDeviceContext->Map(m_ConstBufferLighting.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &subres);
	memcpy(subres.pData, &buffer, sizeof(buffer));
	m_D3DDeviceContext->Unmap(m_ConstBufferLighting.Get(), 0);

	m_D3DDeviceContext->PSSetConstantBuffers(0, 1, m_ConstBufferLighting.GetAddressOf());
}

Hourglass::RenderTarget Hourglass::RenderSystem::CreateRenderTarget(DXGI_FORMAT format, bool createMips) const
{
	RenderTarget rt;

	D3D11_TEXTURE2D_DESC rttDesc;
	rttDesc.Width = m_ClientWidth;
	rttDesc.Height = m_ClientHeight;
	rttDesc.ArraySize = 1;
	rttDesc.Format = format;

	if (m_MsaaEnabled)
	{
		rttDesc.SampleDesc.Count = 4;
		rttDesc.SampleDesc.Quality = m_MsaaQuality - 1;
		rttDesc.MipLevels = 1;
	}
	else
	{
		rttDesc.SampleDesc.Count = 1;
		rttDesc.SampleDesc.Quality = 0;
		rttDesc.MipLevels = createMips ? 0 : 1;
	}

	rttDesc.Usage = D3D11_USAGE_DEFAULT;
	rttDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	rttDesc.CPUAccessFlags = 0;
	rttDesc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS;

	m_D3DDevice->CreateTexture2D(&rttDesc, 0, &rt.Buffer);
	m_D3DDevice->CreateRenderTargetView(rt.Buffer.Get(), 0, &rt.RTV);

	D3D11_SHADER_RESOURCE_VIEW_DESC rtsrvDesc;
	rtsrvDesc.Format = rttDesc.Format;
	rtsrvDesc.ViewDimension = m_MsaaEnabled ? D3D11_SRV_DIMENSION_TEXTURE2DMS : D3D11_SRV_DIMENSION_TEXTURE2D;
	rtsrvDesc.Texture2D.MostDetailedMip = 0;
	rtsrvDesc.Texture2D.MipLevels = createMips ? -1 : 1;

	m_D3DDevice->CreateShaderResourceView(rt.Buffer.Get(), &rtsrvDesc, &rt.SRV);

	if (m_MsaaEnabled)
	{
		rttDesc.SampleDesc.Count = 1;
		rttDesc.SampleDesc.Quality = 0;
		rttDesc.MipLevels = createMips ? 0 : 1;

		m_D3DDevice->CreateTexture2D(&rttDesc, 0, &rt.ResolvedBuffer);

		rtsrvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		m_D3DDevice->CreateShaderResourceView(rt.ResolvedBuffer.Get(), &rtsrvDesc, &rt.ResolvedSRV);
	}

	return rt;
}

//////////////////////////////////////////////////////////////////////////
// Shader creation functions
//////////////////////////////////////////////////////////////////////////

typedef HRESULT(*CreateShaderFunc)(ID3DBlob*, IUnknown**);

enum D3DShaderType
{
	kVertexShader,
	kPixelShader,
	kGeometryShader,
	kComputeShader,

	kD3DShaderTypeCount,
};

static HRESULT RenderDevice_CreateVertexShader(ID3DBlob* pShaderCode, IUnknown** pShader)
{
	return Hourglass::g_RenderDev.device->CreateVertexShader(pShaderCode->GetBufferPointer(), pShaderCode->GetBufferSize(), NULL, (ID3D11VertexShader**)pShader);
}

static HRESULT RenderDevice_CreatePixelShader(ID3DBlob* pShaderCode, IUnknown** pShader)
{
	return Hourglass::g_RenderDev.device->CreatePixelShader(pShaderCode->GetBufferPointer(), pShaderCode->GetBufferSize(), NULL, (ID3D11PixelShader**)pShader);
}

static HRESULT RenderDevice_CreateGeometryShader(ID3DBlob* pShaderCode, IUnknown** pShader)
{
	return Hourglass::g_RenderDev.device->CreateGeometryShader(pShaderCode->GetBufferPointer(), pShaderCode->GetBufferSize(), NULL, (ID3D11GeometryShader**)pShader);
}

static HRESULT RenderDevice_CreateComputeShader(ID3DBlob* pShaderCode, IUnknown** pShader)
{
	return Hourglass::g_RenderDev.device->CreateComputeShader(pShaderCode->GetBufferPointer(), pShaderCode->GetBufferSize(), NULL, (ID3D11ComputeShader**)pShader);
}

static CreateShaderFunc s_CreateShaderFuncs[] =
{
	RenderDevice_CreateVertexShader,
	RenderDevice_CreatePixelShader,
	RenderDevice_CreateGeometryShader,
	RenderDevice_CreateComputeShader,
};

ComPtr<IUnknown> CompilerShader(D3DShaderType type, const char* filename, const char* entrypoint, const char* profile)
{
	// Read shader text from .hlsl
	int fileSize = 0;
	char* pBuffer = Hourglass::FileUtils::ReadBinaryFileBuffer(filename, &fileSize);
	if (!pBuffer)
	{
		assert(0 && "Failed to open shader file");
		return nullptr;
	}

	int shaderCompileFlag = 0;
#if defined(DEBUG) || defined(_DEBUG) || DEBUG_SHADER
	shaderCompileFlag = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif
	HRESULT hr = 0;
	ComPtr<ID3DBlob> pShaderCode = nullptr;
	ComPtr<ID3DBlob> pErrorMsg = nullptr;
	ComPtr<IUnknown> shader = nullptr;

	if (SUCCEEDED(hr = D3DCompile(pBuffer, fileSize, filename, NULL, D3D_COMPILE_STANDARD_FILE_INCLUDE, entrypoint, profile, shaderCompileFlag, 0, &pShaderCode, &pErrorMsg)))
	{
		 HR(s_CreateShaderFuncs[type](pShaderCode.Get(), &shader));
	}

	if (FAILED(hr))
	{
		OutputDebugStringA((char*)pErrorMsg->GetBufferPointer());
		OutputDebugStringA("\n");
		assert(0 && "Failed to compile shader");
	}

	delete[] pBuffer;

	return shader;
}

ComPtr<ID3D11VertexShader> Hourglass::RenderSystem::CompileVertexShader(const char* filename, const char* entrypoint /*= "main" */)
{
	ComPtr<ID3D11VertexShader> vertexShader;
	HR(
		CompilerShader(kVertexShader, filename, entrypoint, "vs_4_0")
		.As<ID3D11VertexShader>(&vertexShader)
	);
	return vertexShader;
}

ComPtr<ID3D11PixelShader> Hourglass::RenderSystem::CompilePixelShader(const char* filename, const char* entrypoint /*= "main"*/)
{
	ComPtr<ID3D11PixelShader> pixelShader;
	HR(
		CompilerShader(kPixelShader, filename, entrypoint, "ps_4_0")
		.As<ID3D11PixelShader>(&pixelShader)
	);
	return pixelShader;
}

ComPtr<ID3D11GeometryShader> Hourglass::RenderSystem::CompileGeometryShader(const char* filename, const char* entrypoint /*= "main"*/)
{
	ComPtr<ID3D11GeometryShader> geometryShader;
	HR(
		CompilerShader(kGeometryShader, filename, entrypoint, "gs_4_0")
		.As<ID3D11GeometryShader>(&geometryShader)
	);
	return geometryShader;
}

ComPtr<ID3D11ComputeShader> Hourglass::RenderSystem::CompileComputeShader(const char* filename, const char* entrypoint /*= "main"*/)
{
	ComPtr<ID3D11ComputeShader> computeShader;
	HR(
		CompilerShader(kComputeShader, filename, entrypoint, "cs_5_0")
		.As<ID3D11ComputeShader>(&computeShader)
	);
	return computeShader;
}
