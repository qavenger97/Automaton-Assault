#include "pch.h"
#include "GPUSort.h"
#include "Renderer/RenderDevice.h"
#include "Core/Shader.h"
HRESULT Hourglass::GPUSort::Init()
{
	auto* device = hg::g_RenderDev.device;
	auto* dc = hg::g_RenderDev.deviceContext;

	D3D11_BUFFER_DESC cbDesc;
	ZeroMemory(&cbDesc, sizeof(cbDesc));
	cbDesc.Usage = D3D11_USAGE_DYNAMIC;
	cbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cbDesc.ByteWidth = 4 * sizeof(UINT);
	device->CreateBuffer(&cbDesc, nullptr, m_dispatchInfoCB.GetAddressOf());

	ID3DBlob* shaderBlob;
	// create shaders
	HRESULT rs = Shader::CompileShader(L"HLSL/SortStepCS2.hlsl", device, &shaderBlob, "cs_5_0");
	if (FAILED(rs))
	{
		return rs;
	}
	rs = device->CreateComputeShader(shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize(), nullptr, &m_pCSSortStep);
	if (FAILED(rs))
	{
		return rs;
	}
	shaderBlob->Release();
	// Create inner sort shader
	const D3D_SHADER_MACRO Defines[2] = { { "SORT_SIZE", "512" },{ nullptr,0 } };
	const D3D_SHADER_MACRO NullDefines[2] = { { nullptr,0 },{ nullptr,0 } };
	rs = Shader::CompileShader(L"HLSL/SortInnerCS.hlsl", device, &shaderBlob, "cs_5_0", Defines);
	if (FAILED(rs))
	{
		return rs;
	}
	rs = device->CreateComputeShader(shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize(), nullptr, &m_pCSSortInner512);
	if (FAILED(rs))
	{
		return rs;
	}
	shaderBlob->Release();

	rs = Shader::CompileShader(L"HLSL/SortCS.hlsl", device, &shaderBlob, "cs_5_0", Defines);
	if (FAILED(rs))
	{
		return rs;
	}
	rs = device->CreateComputeShader(shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize(), nullptr, &m_pCSSort512);
	if (FAILED(rs))
	{
		return rs;
	}
	shaderBlob->Release();

	rs = Shader::CompileShader(L"HLSL/InitSortArgsCS.hlsl", device, &shaderBlob, "cs_5_0");
	if (FAILED(rs))
	{
		return rs;
	}
	rs = device->CreateComputeShader(shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize(), nullptr, &m_pCSInitArgs);
	if (FAILED(rs))
	{
		return rs;
	}
	shaderBlob->Release();

	D3D11_BUFFER_DESC desc;
	ZeroMemory(&desc, sizeof(desc));
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.BindFlags = D3D11_BIND_UNORDERED_ACCESS;
	desc.ByteWidth = 4 * sizeof(UINT);
	desc.MiscFlags = D3D11_RESOURCE_MISC_DRAWINDIRECT_ARGS;
	rs = device->CreateBuffer(&desc, nullptr, &m_pIndirectSortArgsBuffer);
	if (FAILED(rs))
	{
		return rs;
	}
	D3D11_UNORDERED_ACCESS_VIEW_DESC uav;
	ZeroMemory(&uav, sizeof(uav));
	uav.Format = DXGI_FORMAT_R32_UINT;
	uav.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
	uav.Buffer.FirstElement = 0;
	uav.Buffer.NumElements = 4;
	uav.Buffer.Flags = 0;
	rs = device->CreateUnorderedAccessView(m_pIndirectSortArgsBuffer.Get(), &uav, m_pIndirectSortArgsBufferUAV.GetAddressOf());

	return rs;
}

void Hourglass::GPUSort::Execute(unsigned int maxSize, ID3D11UnorderedAccessView * sortBufferUAV, ID3D11Buffer * itemCountBuffer)
{

	auto* dc = hg::g_RenderDev.deviceContext;

	ID3D11UnorderedAccessView* prevUAV = nullptr;
	dc->CSGetUnorderedAccessViews(0, 1, &prevUAV);

	ID3D11Buffer* prevCBs[] = { nullptr, nullptr };
	dc->CSGetConstantBuffers(0, ARRAYSIZE(prevCBs), prevCBs);

	ID3D11Buffer* cbs[] = { itemCountBuffer, m_dispatchInfoCB.Get() };
	dc->CSSetConstantBuffers(0, ARRAYSIZE(cbs), cbs);

	// Write the indirect args to a UAV
	dc->CSSetUnorderedAccessViews(0, 1, m_pIndirectSortArgsBufferUAV.GetAddressOf(), nullptr);

	dc->CSSetShader(m_pCSInitArgs.Get(), nullptr, 0);
	dc->Dispatch(1, 1, 1);

	dc->CSSetUnorderedAccessViews(0, 1, &sortBufferUAV, nullptr);

	bool bDone = sortInitial(maxSize);

	int presorted = 512;
	while (!bDone)
	{
		bDone = sortIncremental(presorted, maxSize);
		presorted *= 2;
	}

	dc->CSSetUnorderedAccessViews(0, 1, &prevUAV, nullptr);
	dc->CSSetConstantBuffers(0, ARRAYSIZE(prevCBs), prevCBs);

	if (prevUAV)
		prevUAV->Release();

	for (size_t i = 0; i < ARRAYSIZE(prevCBs); i++)
		if (prevCBs[i])
			prevCBs[i]->Release();
}

bool Hourglass::GPUSort::sortInitial(unsigned int maxSize)
{
	bool bDone = true;

	unsigned int numThreadGroups = ((maxSize - 1) >> 9) + 1;

	//assert(numThreadGroups <= MAX_NUM_TG);

	if (numThreadGroups>1) bDone = false;
	auto* dc = hg::g_RenderDev.deviceContext;
	// sort all buffers of size 512 (and presort bigger ones)
	dc->CSSetShader(m_pCSSort512.Get(), nullptr, 0);
	dc->DispatchIndirect(m_pIndirectSortArgsBuffer.Get(), 0);

	return bDone;
}

bool Hourglass::GPUSort::sortIncremental(unsigned int presorted, unsigned int maxSize)
{
	bool bDone = true;
	auto* dc = hg::g_RenderDev.deviceContext;
	dc->CSSetShader(m_pCSSortStep.Get(), nullptr, 0);

	// prepare thread group description data
	unsigned int numThreadGroups = 0;

	if (maxSize > presorted)
	{
		if (maxSize>presorted * 2)
			bDone = false;

		unsigned int pow2 = presorted;
		while (pow2<maxSize)
			pow2 *= 2;
		numThreadGroups = pow2 >> 9;
	}

	unsigned int nMergeSize = presorted * 2;
	for (unsigned int nMergeSubSize = nMergeSize >> 1; nMergeSubSize>256; nMergeSubSize = nMergeSubSize >> 1)
		//	for( int nMergeSubSize=nMergeSize>>1; nMergeSubSize>0; nMergeSubSize=nMergeSubSize>>1 ) 
	{
		D3D11_MAPPED_SUBRESOURCE MappedResource;

		dc->Map(m_dispatchInfoCB.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedResource);
		XMINT4* sc = (XMINT4*)MappedResource.pData;
		sc->x = nMergeSubSize;
		if (nMergeSubSize == nMergeSize >> 1)
		{
			sc->y = (2 * nMergeSubSize - 1);
			sc->z = -1;
		}
		else
		{
			sc->y = nMergeSubSize;
			sc->z = 1;
		}
		sc->w = 0;
		dc->Unmap(m_dispatchInfoCB.Get(), 0);

		dc->Dispatch(numThreadGroups, 1, 1);
	}

	dc->CSSetShader(m_pCSSortInner512.Get(), nullptr, 0);
	dc->Dispatch(numThreadGroups, 1, 1);

	return bDone;
}
