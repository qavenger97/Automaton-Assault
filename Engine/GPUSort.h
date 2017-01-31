#pragma once
namespace Hourglass
{
	class GPUSort
	{
	public:
		virtual ~GPUSort() {}

		HRESULT Init();
		void Execute(unsigned int maxSize, ID3D11UnorderedAccessView* sortBufferUAV, ID3D11Buffer* itemCountBuffer);

	private:
		bool sortInitial(unsigned int maxSize);
		bool sortIncremental(unsigned int presorted, unsigned int maxSize);

	private:
		ComPtr<ID3D11Buffer>					m_dispatchInfoCB;		// constant buffer containing dispatch specific information

		ComPtr<ID3D11ComputeShader>				m_pCSSortStep;			// CS port of the VS/PS bitonic sort
		ComPtr<ID3D11ComputeShader>				m_pCSSort512;			// CS implementation to sort a number of 512 element sized arrays using a single dispatch
		ComPtr<ID3D11ComputeShader>				m_pCSSortInner512;		// CS implementation of the "down" pass from 512 to 1
		ComPtr<ID3D11ComputeShader>				m_pCSInitArgs;			// CS to write indirect args for Dispatch calls

		ComPtr<ID3D11Buffer>					m_pIndirectSortArgsBuffer;
		ComPtr<ID3D11UnorderedAccessView>		m_pIndirectSortArgsBufferUAV;
	};

}
