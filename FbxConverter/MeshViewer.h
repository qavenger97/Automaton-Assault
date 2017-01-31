#pragma once

#include "pch.h"
#include "Core\RenderWindow.h"
#include "Renderer\Mesh.h"

class MeshViewer
{
public:
	void Start();

private:
	bool InitD3D11();
	void ShowOpenMeshDialog();

	void CreateRenderTargetView();
	void CreateDepthStencilBufferAndView();
	void SetupViewport();
	void CreateVertexDeclarations();
	void CreateConstantBuffers();
	bool HandleWindowMessage();

	void RenderMesh();

	void SetupModelMatrix();

	void SetupCamera();

	float GetClientAspect() const;
private:
	hg::RenderWindow m_RenderWindow;
	UINT m_ClientWidth;
	UINT m_ClientHeight;

	ComPtr<ID3D11Device>				m_D3DDevice;
	ComPtr<ID3D11DeviceContext>			m_D3DDeviceContext;
	ComPtr<IDXGISwapChain>				m_SwapChain;

	ComPtr<ID3D11RenderTargetView>		m_RenderTargetView;
	ComPtr<ID3D11Texture2D>				m_DepthStencilBuffer;
	ComPtr<ID3D11DepthStencilView>		m_DepthStencilView;

	ComPtr<ID3D11VertexShader>			m_ColorVertexShader;
	ComPtr<ID3D11PixelShader>			m_ColorPixelShader;
	ComPtr<ID3D11VertexShader>			m_MeshVertexShader;
	ComPtr<ID3D11PixelShader>			m_MeshPixelShader;

	ComPtr<ID3D11Buffer>				m_cbScene;
	ComPtr<ID3D11Buffer>				m_cbObject;

	hg::Mesh	m_Mesh;
};

