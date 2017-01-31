/*!
 * \file RenderSystem.h
 *
 * \author Shiyang Ao
 * \date October 2016
 *
 * Render system class
 */
#pragma once

#include "../Component/Transform.h"
#include "../Core/ComponentPool.h"
#include "Core/DevMenu.h"
#include "../Game/HLSL/ConstBufferVS.h"
#include "../Game/HLSL/ConstBufferPS.h"
#include "../System/ISystem.h"
#include "CommonDef.h"
#include "RenderBuffer.h"
#include "RenderComponent.h"
#include "RenderPass.h"
#include "VertexDeclaration.h"
#include "Light.h"
namespace Hourglass
{
	struct Pose;

	struct RenderTarget 
	{
		ComPtr<ID3D11Texture2D>				Buffer;
		ComPtr<ID3D11RenderTargetView>		RTV;
		ComPtr<ID3D11ShaderResourceView>	SRV;

		// For msaa, we must resolve render target before it can be used as a shader resource view
		ComPtr<ID3D11Texture2D>				ResolvedBuffer;
		ComPtr<ID3D11ShaderResourceView>	ResolvedSRV;
	};

	struct UnorderedAccessView
	{
		ComPtr<ID3D11Texture2D>				Buffer;
	};

	class RenderSystem : public ISystem
	{
	public:
		// Initialize render system
		bool Init(HWND hWnd, int width, int height, bool useMsaa);

		// Shutdown render system
		void Shutdown();

		// Draw all objects in the scene
		void Render();

		void SetPoses( const Pose* poses ) { m_Poses = poses; }

		// Get render window aspect ratio
		float GetClientAspect() const;

		// Windows resize event handler
		void OnResize(int width, int height);
		void TogglePostProcess();
		void ToggleVsync();
		void SetFlags(bool enable);
		static ComPtr<ID3D11VertexShader>	CompileVertexShader		(const char* filename, const char* entrypoint = "main");
		static ComPtr<ID3D11PixelShader>	CompilePixelShader		(const char* filename, const char* entrypoint = "main");
		static ComPtr<ID3D11GeometryShader>	CompileGeometryShader	(const char* filename, const char* entrypoint = "main");
		static ComPtr<ID3D11ComputeShader>	CompileComputeShader	(const char* filename, const char* entrypoint = "main");

	private:
		//////////////////////////////////////////////////////////////////////////
		// Initialization methods
		//////////////////////////////////////////////////////////////////////////
		void CreateBackBufferRTV();
		void CreateDepthStencilBufferAndView();
		void SetupViewport();
		void CreateVertexDeclarations();
		void CreateShaders();
		void CreateConstantBuffers();
		void CreateSamplerState();
		void CreateBlendState();

		void CreateScreenQuad();
		void CreatePostProcessorResources();

		void ResizeSwapChain(int width, int height);

		std::vector<Light*> BuildVisibleLightList(const BoundingFrustum& frustum);

		void RenderPreDepthPass();

		void SetShaderPoseConstBuffer( const Hourglass::Pose & buffer );

		void SetShaderObjectConstBuffer(const SHADER_OBJECT_BUFFER& buffer);

		void SetShaderSceneConstBuffer(const SHADER_SCENE_BUFFER& buffer);
		void SetShaderLightConstBuffer(const SHADER_LIGHT_BUFFER& buffer);

		RenderTarget CreateRenderTarget(DXGI_FORMAT format, bool createMips) const;

		int m_ClientWidth;			// Client window width
		int m_ClientHeight;			// Client window height
		bool m_MsaaEnabled;
		UINT m_MsaaQuality;			// Max MSAA quality supported
		TCHAR* m_AdapterName;		// Graphics card name

		ComPtr<ID3D11Device>				m_D3DDevice;
		ComPtr<ID3D11DeviceContext>			m_D3DDeviceContext;
		ComPtr<IDXGISwapChain>				m_SwapChain;
		ComPtr<ID3D11RenderTargetView>		m_BackBufferRTV;
		ComPtr<ID3D11Texture2D>				m_DepthStencilBuffer;
		ComPtr<ID3D11DepthStencilView>		m_DepthStencilView;

		RenderTarget						m_EmissiveRT;
		RenderBuffer						m_ScreenQuad;

		ComPtr<ID3D11Texture2D>				m_PostProcessorOutputBuffer;
		ComPtr<ID3D11UnorderedAccessView>	m_PostProcessorOutputUAV;
		ComPtr<ID3D11ShaderResourceView>	m_PostProcessorOutputSRV;
		RenderQueue	m_RenderQueue;

		ComPtr<ID3D11Buffer>	m_ConstBufferPerObject;
		ComPtr<ID3D11Buffer>	m_ConstBufferPerScene;
		ComPtr<ID3D11Buffer>	m_ConstBufferPose;
		ComPtr<ID3D11Buffer>	m_ConstBufferLighting;

		ComPtr<ID3D11DepthStencilState>		m_DepthStencilState[kDepthStateCount];

		UINT	m_DrawCallCount;

		const Pose* m_Poses;

	private:

		DECLARE_COMPONENT_POOL( PrimitiveRenderer, 2048 );
		DECLARE_COMPONENT_POOL( MeshRenderer, 2048 );
		DECLARE_COMPONENT_POOL( SkinnedMeshRenderer, 64 );
		DECLARE_COMPONENT_POOL( Light, 2048 );
	};
}

