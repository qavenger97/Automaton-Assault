#include "MeshViewer.h"

#include "Color_VS.csh"
#include "Color_PS.csh"
#include "Mesh_VS.csh"
#include "Mesh_PS.csh"
#include "Core\FileArchive.h"
#include <commdlg.h>
#include <shobjidl.h>
#include "Renderer\RenderDevice.h"

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_DESTROY:
		// share post quit message with WM_CLOSE
	case WM_CLOSE:
	{
		PostQuitMessage(0);
		break;
	}

	case WM_SIZE:
	{
		break;
	}

	case WM_KEYDOWN:
	{
		break;
	}

	case WM_KEYUP:
	{
		break;
	}

	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}

	return 0;
}


void MeshViewer::Start()
{
	m_ClientWidth = 1024;
	m_ClientHeight = 768;

	SetProcessDPIAware();

	m_RenderWindow.Create(WndProc, m_ClientWidth, m_ClientHeight);

	InitD3D11();

	// Create render target view
	CreateRenderTargetView();

	// Create depth/stencil buffer and view
	CreateDepthStencilBufferAndView();

	// Bind views to the output merger stage
	m_D3DDeviceContext->OMSetRenderTargets(1, m_RenderTargetView.GetAddressOf(), m_DepthStencilView.Get());

	// Setup viewport
	SetupViewport();

	CreateVertexDeclarations();

	CreateConstantBuffers();

	ShowOpenMeshDialog();

	m_D3DDevice->CreateVertexShader(Color_VS, sizeof(Color_VS), nullptr, &m_ColorVertexShader);
	m_D3DDevice->CreatePixelShader(Color_PS, sizeof(Color_PS), nullptr, &m_ColorPixelShader);
	m_D3DDevice->CreateVertexShader(Mesh_VS, sizeof(Mesh_VS), nullptr, &m_MeshVertexShader);
	m_D3DDevice->CreatePixelShader(Mesh_PS, sizeof(Mesh_PS), nullptr, &m_MeshPixelShader);

	while (HandleWindowMessage())
	{
		RenderMesh();

		m_SwapChain->Present(1, 0);
	}

	m_RenderWindow.Destroy();
}

bool MeshViewer::InitD3D11()
{
	UINT createDeviceFlags = 0;

#if defined(DEBUG) || defined(_DEBUG)
	createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	// Create DXGI factory interface
	ComPtr<IDXGIFactory1> dxgiFactory = 0;
	CreateDXGIFactory1(__uuidof(IDXGIFactory1), (void**)dxgiFactory.GetAddressOf());

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

	hg::g_RenderDev.device = m_D3DDevice.Get();
	hg::g_RenderDev.deviceContext = m_D3DDeviceContext.Get();

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

	DXGI_SWAP_CHAIN_DESC sd;
	sd.BufferDesc.Width = m_ClientWidth;
	sd.BufferDesc.Height = m_ClientHeight;
	sd.BufferDesc.RefreshRate.Numerator = 60;
	sd.BufferDesc.RefreshRate.Denominator = 1;
	sd.BufferDesc.Format = backbuffer_format;
	sd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	sd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	sd.SampleDesc.Count = 1;
	sd.SampleDesc.Quality = 0;

	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.BufferCount = 1;
	sd.OutputWindow = m_RenderWindow.GetHwnd();
	sd.Windowed = true;
	sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	sd.Flags = 0;

	// Create swap chain
	dxgiFactory->CreateSwapChain(m_D3DDevice.Get(), &sd, &m_SwapChain);

	return true;
}

void MeshViewer::ShowOpenMeshDialog()
{
	// open a file name
	OPENFILENAMEA ofn;
	char szFileName[MAX_PATH];

	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = NULL;
	ofn.lpstrFile = szFileName;
	ofn.lpstrFile[0] = '\0';
	ofn.nMaxFile = sizeof(szFileName);
	ofn.lpstrFilter = ("Hourglass Models (*.hmdl)\0*.HMDL\0All\0*.*\0");
	ofn.nFilterIndex = 1;
	ofn.lpstrFileTitle = NULL;
	ofn.nMaxFileTitle = 0;
	ofn.lpstrInitialDir = NULL;
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

	GetOpenFileNameA(&ofn);

	hg::FileArchive archive;

	if (archive.Open(szFileName, hg::kFileOpenMode_Read))
	{
		m_Mesh.Serialize(archive);
		m_Mesh.SetType(hg::Mesh::kStandard);
		m_Mesh.UpdateRenderBuffer();

		archive.Close();
	}
}

void MeshViewer::CreateRenderTargetView()
{
	ComPtr<ID3D11Texture2D> backBuffer;
	m_SwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D),
		reinterpret_cast<void**>(backBuffer.GetAddressOf()));
	m_D3DDevice->CreateRenderTargetView(backBuffer.Get(), 0, &m_RenderTargetView);
}

void MeshViewer::CreateDepthStencilBufferAndView()
{
	D3D11_TEXTURE2D_DESC depthStencilDesc;
	depthStencilDesc.Width = m_ClientWidth;
	depthStencilDesc.Height = m_ClientHeight;
	depthStencilDesc.MipLevels = 1;
	depthStencilDesc.ArraySize = 1;
	depthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilDesc.SampleDesc.Count = 1;
	depthStencilDesc.SampleDesc.Quality = 0;

	depthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
	depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthStencilDesc.CPUAccessFlags = 0;
	depthStencilDesc.MiscFlags = 0;

	m_D3DDevice->CreateTexture2D(&depthStencilDesc, 0, &m_DepthStencilBuffer);
	m_D3DDevice->CreateDepthStencilView(m_DepthStencilBuffer.Get(), 0, &m_DepthStencilView);
}

void MeshViewer::SetupViewport()
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

void MeshViewer::CreateVertexDeclarations()
{
	// Colored vertex
	D3D11_INPUT_ELEMENT_DESC colorVertexDesc[] =
	{
		{ "POSITION",	0, DXGI_FORMAT_R32G32B32A32_FLOAT,	0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR",		0, DXGI_FORMAT_R32G32B32A32_FLOAT,	0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	m_D3DDevice->CreateInputLayout(colorVertexDesc, 2, Color_VS, sizeof(Color_VS), &hg::g_InputLayouts[hg::kVertexDecl_Color]);

	// Mesh vertex
	D3D11_INPUT_ELEMENT_DESC meshVertexDesc[] =
	{
		{ "POSITION",	0, DXGI_FORMAT_R32G32B32_FLOAT,		0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD",	0, DXGI_FORMAT_R32G32_FLOAT,		0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL",		0, DXGI_FORMAT_R32G32B32_FLOAT,		0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TANGENT",	0, DXGI_FORMAT_R32G32B32A32_FLOAT,	0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	m_D3DDevice->CreateInputLayout(meshVertexDesc, 4, Mesh_VS, sizeof(Mesh_VS), &hg::g_InputLayouts[hg::kVertexDecl_PosUV0NormTan]);
}


void MeshViewer::CreateConstantBuffers()
{
	D3D11_BUFFER_DESC cbDesc;
	ZeroMemory(&cbDesc, sizeof(cbDesc));
	cbDesc.ByteWidth = sizeof(XMFLOAT4X4);
	cbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cbDesc.Usage = D3D11_USAGE_DYNAMIC;

	{
		m_D3DDevice->CreateBuffer(&cbDesc, NULL, &m_cbScene);

		// Initialize buffer values with zero
		XMFLOAT4X4 buffer;
		ZeroMemory(&buffer, sizeof(buffer));

		D3D11_MAPPED_SUBRESOURCE subres;
		m_D3DDeviceContext->Map(m_cbScene.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &subres);
		memcpy(subres.pData, &buffer, sizeof(buffer));
		m_D3DDeviceContext->Unmap(m_cbScene.Get(), 0);
	}

	{
		m_D3DDevice->CreateBuffer(&cbDesc, NULL, &m_cbObject);

		// Initialize buffer values with zero
		XMFLOAT4X4 buffer;
		ZeroMemory(&buffer, sizeof(buffer));

		D3D11_MAPPED_SUBRESOURCE subres;
		m_D3DDeviceContext->Map(m_cbObject.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &subres);
		memcpy(subres.pData, &buffer, sizeof(buffer));
		m_D3DDeviceContext->Unmap(m_cbObject.Get(), 0);
	}

}

bool MeshViewer::HandleWindowMessage()
{
	MSG msg;

	while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
	{
		if (msg.message == WM_QUIT)
		{
			return false;
		}
		else
		{
			// Translate the message and dispatch it to WndProc()
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return true;
}

void MeshViewer::RenderMesh()
{
	FLOAT clearColor[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
	m_D3DDeviceContext->ClearRenderTargetView(m_RenderTargetView.Get(), clearColor);
	m_D3DDeviceContext->ClearDepthStencilView(m_DepthStencilView.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0);

	m_D3DDeviceContext->VSSetShader(m_MeshVertexShader.Get(), nullptr, 0);
	m_D3DDeviceContext->PSSetShader(m_MeshPixelShader.Get(), nullptr, 0);

	SetupCamera();
	SetupModelMatrix();

	for (int i = 0; i < m_Mesh.Submeshes.size(); i++)
	{
		hg::SubmeshData& sd = m_Mesh.Submeshes[i];
		m_Mesh.GetRenderBuffer()->DrawIndexed(sd.indexCount, sd.startIndexOffset, sd.baseVertexOffset);
	}
}

void MeshViewer::SetupModelMatrix()
{
	XMFLOAT4X4 buffer;

	XMStoreFloat4x4(&buffer, XMMatrixIdentity());

	D3D11_MAPPED_SUBRESOURCE subres;
	m_D3DDeviceContext->Map(m_cbObject.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &subres);
	memcpy(subres.pData, &buffer, sizeof(buffer));
	m_D3DDeviceContext->Unmap(m_cbObject.Get(), 0);

	m_D3DDeviceContext->VSSetConstantBuffers(0, 1, m_cbObject.GetAddressOf());
}

void MeshViewer::SetupCamera()
{
	XMFLOAT4X4 buffer;

	hg::Aabb aabb = m_Mesh.GetAabb();
	float dist = (aabb.pMax - aabb.pMin).Length();
	XMVECTOR camPos = XMVector3Normalize(XMVectorSet(0, 8, -8, 1)) * dist * 1.2f;

	XMMATRIX viewMatrix = XMMatrixLookAtLH(camPos, XMVectorSet(0, 0, 0, 1), XMVectorSet(0, 1, 0, 1));
	XMMATRIX projMatrix = XMMatrixPerspectiveFovLH(XMConvertToRadians(75.0f), GetClientAspect(), 0.001f, 1000.0f);
	XMStoreFloat4x4(&buffer, XMMatrixMultiply(viewMatrix, projMatrix));

	D3D11_MAPPED_SUBRESOURCE subres;
	m_D3DDeviceContext->Map(m_cbScene.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &subres);
	memcpy(subres.pData, &buffer, sizeof(buffer));
	m_D3DDeviceContext->Unmap(m_cbScene.Get(), 0);

	m_D3DDeviceContext->VSSetConstantBuffers(1, 1, m_cbScene.GetAddressOf());
}

float MeshViewer::GetClientAspect() const
{
	return (float)m_ClientWidth / m_ClientHeight;
}
