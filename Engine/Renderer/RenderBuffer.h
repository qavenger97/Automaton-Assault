#pragma once

namespace Hourglass
{
	// A wrapper class for hardware vertex and index buffers
	class RenderBuffer
	{
	protected:
		ComPtr<ID3D11Buffer>	m_VertexBuffer;
		ComPtr<ID3D11Buffer>	m_IndexBuffer;
		ID3D11InputLayout*	m_InputLayout;

		UINT				m_Stride;
		UINT				m_VertexCount;
		UINT				m_IndexCount;

		uint16_t			m_PrimitiveTopology;
	public:
		RenderBuffer();

		// Get vertex count in this render buffer
		UINT GetVertexCount() const { return m_VertexCount; }

		// Get index count in this render buffer
		UINT GetIndexCount() const { return m_IndexCount; }


		// Check if vertex buffer has been initialized
		bool HasVertexBuffer() const { return m_VertexBuffer != nullptr; }

		// Check if index buffer has been initialized
		bool HasIndexBuffer() const { return m_IndexBuffer != nullptr; }

		void CreateVertexBuffer(void* data, UINT vertexTypeSize, UINT vertexCount, D3D11_PRIMITIVE_TOPOLOGY topology, ID3D11InputLayout* inputLayout, bool dynamic = false, const char* debugResourceName = nullptr);
		void CreateIndexBuffer(void* data, UINT indexTypeSize, UINT indexCount, bool dynamic = false, const char* debugResourceName = nullptr);

		// Update vertex buffer content
		//		Note: To update vertex buffer, it must be created as dynamic buffer to avoid performance issue
		void UpdateDynamicVertexBuffer(void* data, UINT vertexTypeSize, UINT vertexCount);

		void Draw();
		void DrawIndexed(UINT IndexCount, UINT StartIndexLocation, INT BaseVertexLocation);
		void DrawInstanced(int instanceCount);

		// Release both buffers
		void Reset();
	};
}