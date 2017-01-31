/*!
 * \file RenderQueue.h
 *
 * \author Shiyang Ao
 * \date October 2016
 *
 * Render queue class that gathers all necessary information for render pipeline
 */
#pragma once

#include "RenderPass.h"
#include "RenderBuffer.h"
#include "../Component/Transform.h"
#include "Mesh.h"
#include "CommonDef.h"
#include "../Game/HLSL/ConstBufferPS.h"

namespace Hourglass
{
	class Light;

	struct RenderTechniqueData
	{
		// TODO: Blend, depth state or so

		ID3D11VertexShader*		m_VertexShader;
		ID3D11PixelShader*		m_PixelShader;

		// TODO: More shaders here
	};

	struct RenderMaterialData
	{
		int							m_BlendMode;
		ID3D11ShaderResourceView*	m_ShaderResourceView[8];
		SamplerStateType			m_SamplerState;
		bool						m_UseUniformColor;
		Color						m_UniformColor;
	};

	class RenderPass;
	
	struct RenderShape
	{
		RenderBuffer*			buffer;
		SubmeshData*			submesh;
		UINT					lightCount;
		Light*					lights[MAX_POINT_LIGHT_PER_OBJECT];
		bool					useMatrix;		// Use matrix instead of a transform for this shape

		// Should use either transform or matrix for a render shape
		Transform const*		transform;
		Matrix					matrix;

		bool					useAnimation;
		int						animationId;  // by default, no animation
	};

	struct DrawCallData
	{
		RenderTechniqueData technique;
		RenderMaterialData	material;
		RenderShape		    shape;
	};

	class RenderMaterial
	{
	public:
		void AddRenderShape(const DrawCallData& drawCallData);
		void ClearRenderShapes();

		bool Equals(const RenderMaterialData& data) const;

		RenderMaterialData			m_MaterialData;
		std::vector<RenderShape>	m_RenderShapes;
	};

	//////////////////////////////////////////////////////////////////////////

	class RenderTechnique
	{
	public:
		void AddRenderShape(const DrawCallData& drawCallData);
		void ClearRenderQueueData();

		bool Equals(const RenderTechniqueData& data) const;

		RenderTechniqueData				m_TechniqueData;
		std::vector<RenderMaterial>		m_Materials;
	};

	//////////////////////////////////////////////////////////////////////////

	class RenderPass
	{
	public:
		void AddRenderTechnique(const DrawCallData& drawCallData);

		// Clear all render techniques from this render pass
		void ClearRenderTechniques();

		std::vector<RenderTechnique>	m_RenderTechniques;
	};

	//////////////////////////////////////////////////////////////////////////

	class RenderQueue
	{
	public:

		void AddRenderShape(RenderPassType pass, const DrawCallData& drawCallData);
		void AddDepthPassShape(const RenderShape& shape);

		// Clear render queue
		void Clear();

		RenderPass m_RenderPasses[kRenderPassCount];
		std::vector<RenderShape>	m_DepthPassShapes;
	};

}