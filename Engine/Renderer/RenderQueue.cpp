/*!
 * \file RenderQueue.cpp
 *
 * \author Shiyang Ao
 * \date October 2016
 *
 * Render queue
 */
#include "pch.h"

#include "RenderQueue.h"

//////////////////////////////////////////////////////////////////////////
// RenderMaterial methods
//////////////////////////////////////////////////////////////////////////

void Hourglass::RenderMaterial::AddRenderShape(const DrawCallData& drawCallData)
{
	// TODO: Save draw calls by combining same buffers
	m_RenderShapes.push_back(drawCallData.shape);
}


void Hourglass::RenderMaterial::ClearRenderShapes()
{
	m_RenderShapes.clear();
}

bool Hourglass::RenderMaterial::Equals(const RenderMaterialData& data) const
{
	return memcmp(&m_MaterialData, &data, sizeof(RenderMaterialData)) == 0;
}

//////////////////////////////////////////////////////////////////////////
// RenderTechnique methods
//////////////////////////////////////////////////////////////////////////

void Hourglass::RenderTechnique::AddRenderShape(const DrawCallData& drawCallData)
{
	int i = 0;
	for (; i < m_Materials.size(); i++)
	{
		if (m_Materials[i].Equals(drawCallData.material))
		{
			m_Materials[i].AddRenderShape(drawCallData);
			return;
		}
	}

	m_Materials.push_back(RenderMaterial{ drawCallData.material });
	m_Materials[i].AddRenderShape(drawCallData);
}


void Hourglass::RenderTechnique::ClearRenderQueueData()
{
	for (int i = 0; i < m_Materials.size(); i++)
	{
		m_Materials[i].ClearRenderShapes();
	}
}

bool Hourglass::RenderTechnique::Equals(const RenderTechniqueData& data) const
{
	return memcmp(&m_TechniqueData, &data, sizeof(RenderTechniqueData)) == 0;
}

//////////////////////////////////////////////////////////////////////////
// RenderPass methods
//////////////////////////////////////////////////////////////////////////

void Hourglass::RenderPass::AddRenderTechnique(const DrawCallData& drawCallData)
{
	int i = 0;
	for (; i < m_RenderTechniques.size(); i++)
	{
		if (m_RenderTechniques[i].Equals(drawCallData.technique))
		{
			m_RenderTechniques[i].AddRenderShape(drawCallData);
			return;
		}
	}

	m_RenderTechniques.push_back(RenderTechnique{ drawCallData.technique });
	m_RenderTechniques[i].AddRenderShape(drawCallData);
}

void Hourglass::RenderPass::ClearRenderTechniques()
{
	for (size_t i = 0; i < m_RenderTechniques.size(); i++)
	{
		m_RenderTechniques[i].ClearRenderQueueData();
	}
}

//////////////////////////////////////////////////////////////////////////
// RenderQueue methods
//////////////////////////////////////////////////////////////////////////

void Hourglass::RenderQueue::AddRenderShape(RenderPassType pass, const DrawCallData& drawCallData)
{
	m_RenderPasses[pass].AddRenderTechnique(drawCallData);
}

void Hourglass::RenderQueue::AddDepthPassShape(const RenderShape& shape)
{
	m_DepthPassShapes.push_back(shape);
}

void Hourglass::RenderQueue::Clear()
{
	m_DepthPassShapes.clear();

	for (int i = 0; i < kRenderPassCount; i++)
	{
		m_RenderPasses[i].ClearRenderTechniques();
	}
}
