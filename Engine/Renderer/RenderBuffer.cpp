
#include "pch.h"

#include "RenderBuffer.h"
#include "D3DUtil.h"
#include "RenderDevice.h"

Hourglass::RenderBuffer::RenderBuffer()
	: m_InputLayout(nullptr), m_PrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_UNDEFINED)
{

}

void Hourglass::RenderBuffer::CreateVertexBuffer(void* data, UINT vertexTypeSize, UINT vertexCount, D3D11_PRIMITIVE_TOPOLOGY topology, ID3D11InputLayout* inputLayout, bool dynamic /*= false*/, const char* debugResourceName /*= nullptr*/)
{
	m_PrimitiveTopology = topology;
	m_InputLayout = inputLayout;

	D3D11_BUFFER_DESC vbd;
	ZeroMemory(&vbd, sizeof(vbd));
	vbd.Usage = dynamic ? D3D11_USAGE_DYNAMIC : D3D11_USAGE_DEFAULT;
	vbd.ByteWidth = vertexTypeSize * vertexCount;
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;

	if (dynamic)
	{
		vbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	}

	if (data)
	{
		D3D11_SUBRESOURCE_DATA initVertexData;
		ZeroMemory(&initVertexData, sizeof(initVertexData));
		initVertexData.pSysMem = data;

		g_RenderDev.device->CreateBuffer(&vbd, &initVertexData, &m_VertexBuffer);
	}
	else
	{
		g_RenderDev.device->CreateBuffer(&vbd, NULL, &m_VertexBuffer);
	}

	m_VertexCount = vertexCount;
	m_Stride = vertexTypeSize;

#if _DEBUG
	if (debugResourceName)
		m_VertexBuffer->SetPrivateData(WKPDID_D3DDebugObjectName, (UINT)strlen(debugResourceName), debugResourceName);
#endif
}

void Hourglass::RenderBuffer::CreateIndexBuffer(void* data, UINT indexTypeSize, UINT indexCount, bool dynamic /*= false*/, const char* debugResourceName /*= nullptr*/)
{
	D3D11_BUFFER_DESC ibd;
	ZeroMemory(&ibd, sizeof(ibd));
	ibd.Usage = dynamic ? D3D11_USAGE_DYNAMIC : D3D11_USAGE_DEFAULT;
	ibd.ByteWidth = indexTypeSize * indexCount;
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;

	D3D11_SUBRESOURCE_DATA initIndexData;
	ZeroMemory(&initIndexData, sizeof(initIndexData));
	initIndexData.pSysMem = data;

	g_RenderDev.device->CreateBuffer(&ibd, &initIndexData, &m_IndexBuffer);
	m_IndexCount = indexCount;

#if _DEBUG
	if (debugResourceName)
		m_IndexBuffer->SetPrivateData(WKPDID_D3DDebugObjectName, (UINT)strlen(debugResourceName), debugResourceName);
#endif
}

void Hourglass::RenderBuffer::UpdateDynamicVertexBuffer(void* data, UINT vertexTypeSize, UINT vertexCount)
{
	assert(m_VertexBuffer && "Vertex buffer is not initialized.");

	D3D11_MAPPED_SUBRESOURCE subres;
	g_RenderDev.deviceContext->Map(m_VertexBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &subres);
	memcpy(subres.pData, data, vertexTypeSize * vertexCount);
	g_RenderDev.deviceContext->Unmap(m_VertexBuffer.Get(), 0);

	m_VertexCount = vertexCount;
}

void Hourglass::RenderBuffer::Draw()
{
	assert(m_VertexBuffer && "Vertex buffer is not initialized.");

	UINT offset = 0;

	if (m_IndexBuffer)
	{
		assert(m_InputLayout);
		g_RenderDev.deviceContext->IASetInputLayout(m_InputLayout);
		g_RenderDev.deviceContext->IASetVertexBuffers(0, 1, m_VertexBuffer.GetAddressOf(), &m_Stride, &offset);
		g_RenderDev.deviceContext->IASetIndexBuffer(m_IndexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
		g_RenderDev.deviceContext->IASetPrimitiveTopology((D3D11_PRIMITIVE_TOPOLOGY)m_PrimitiveTopology);
		g_RenderDev.deviceContext->DrawIndexed(m_IndexCount, 0, 0);
	}
	else if (m_VertexBuffer)
	{
		assert(m_InputLayout);
		g_RenderDev.deviceContext->IASetInputLayout(m_InputLayout);
		g_RenderDev.deviceContext->IASetVertexBuffers(0, 1, m_VertexBuffer.GetAddressOf(), &m_Stride, &offset);
		g_RenderDev.deviceContext->IASetPrimitiveTopology((D3D11_PRIMITIVE_TOPOLOGY)m_PrimitiveTopology);
		g_RenderDev.deviceContext->Draw(m_VertexCount, 0);
	}
}

void Hourglass::RenderBuffer::DrawIndexed(UINT IndexCount, UINT StartIndexLocation, INT BaseVertexLocation)
{
	assert(m_VertexBuffer && "Vertex buffer is not initialized.");

	UINT offset = 0;

	if (m_IndexBuffer)
	{
		assert(m_InputLayout);
		g_RenderDev.deviceContext->IASetInputLayout(m_InputLayout);
		g_RenderDev.deviceContext->IASetVertexBuffers(0, 1, m_VertexBuffer.GetAddressOf(), &m_Stride, &offset);
		g_RenderDev.deviceContext->IASetIndexBuffer(m_IndexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
		g_RenderDev.deviceContext->IASetPrimitiveTopology((D3D11_PRIMITIVE_TOPOLOGY)m_PrimitiveTopology);
		g_RenderDev.deviceContext->DrawIndexed(IndexCount, StartIndexLocation, BaseVertexLocation);
	}
}

void Hourglass::RenderBuffer::DrawInstanced(int instanceCount)
{
	assert(m_VertexBuffer && "Vertex buffer is not initialized.");

	UINT offset = 0;

	if (m_IndexBuffer)
	{
		g_RenderDev.deviceContext->IASetInputLayout(m_InputLayout);
		g_RenderDev.deviceContext->IASetVertexBuffers(0, 1, m_VertexBuffer.GetAddressOf(), &m_Stride, &offset);
		g_RenderDev.deviceContext->IASetIndexBuffer(m_IndexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
		g_RenderDev.deviceContext->IASetPrimitiveTopology((D3D11_PRIMITIVE_TOPOLOGY)m_PrimitiveTopology);
		g_RenderDev.deviceContext->DrawIndexedInstanced(m_IndexCount, instanceCount, 0, 0, 0);
	}
	else if (m_VertexBuffer)
	{
		g_RenderDev.deviceContext->IASetInputLayout(m_InputLayout);
		g_RenderDev.deviceContext->IASetVertexBuffers(0, 1, m_VertexBuffer.GetAddressOf(), &m_Stride, &offset);
		g_RenderDev.deviceContext->IASetPrimitiveTopology((D3D11_PRIMITIVE_TOPOLOGY)m_PrimitiveTopology);
		g_RenderDev.deviceContext->DrawInstanced(m_VertexCount, instanceCount, 0, 0);
	}
}

void Hourglass::RenderBuffer::Reset()
{
	m_InputLayout = nullptr;
	m_VertexBuffer = nullptr;
	m_IndexBuffer = nullptr;
	m_VertexCount = 0;
	m_IndexCount = 0;
}
