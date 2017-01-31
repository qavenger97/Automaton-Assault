/*!
 * \file RenderComponent.h
 *
 * \author Shiyang Ao
 * \date November 2016
 *
 * 
 */
#pragma once

#include "../Component/IComponent.h"
#include "RenderBuffer.h"
#include "RenderQueue.h"

namespace Hourglass
{
	class Mesh;
	class Material;
	class Texture;
	class Light;

	enum MaterialRenderType
	{
		kRenderType_Default,
		kRenderType_Pbr,
		kRenderType_Barrier,
		kRenderType_Unlit,
		kRenderType_PbrFlow,
	};

	// Base render component class
	class RenderComponent : public IComponent
	{
	public:
		// Send this render component to render queue for rendering
		virtual void AddToRenderQueue(RenderQueue& renderQueue, const std::vector<Light*>& lightList) = 0;

		void SetColor(const Color& color) { m_MeshColor = color; }
		Color GetColor() const { return m_MeshColor; }

		// Get aabb of this component in world space for frustum culling
		virtual Aabb GetRenderAabb() const = 0;

	protected:
		void DrawCall_FetchLightingInfo(DrawCallData& drawCallData, const std::vector<Light*>& lightList) const;
		void DrawCall_AssignMaterial(DrawCallData& drawCallData, const Material* material) const;

	protected:
		Color		m_MeshColor;
	};

	//////////////////////////////////////////////////////////////////////////

	// Primitive renderer class is used for rendering primitive shapes
	class PrimitiveRenderer : public RenderComponent
	{
		DECLARE_COMPONENT_TYPEID
	public:
		//////////////////////////////////////////////////////////////////////////
		// Override IComponent functions
		//////////////////////////////////////////////////////////////////////////
		void LoadFromXML( tinyxml2::XMLElement* data );

		/**
		* Make a copy of this component
		*/
		virtual IComponent* MakeCopyDerived() const;

		//////////////////////////////////////////////////////////////////////////
		// Override RenderComponent functions
		//////////////////////////////////////////////////////////////////////////

		void AddToRenderQueue(RenderQueue& renderQueue, const std::vector<Light*>& lightList);

		Aabb GetRenderAabb() const;

		//////////////////////////////////////////////////////////////////////////
		// PrimitiveRenderer functions
		//////////////////////////////////////////////////////////////////////////
		void CreateColoredBox(const Color& color, float scale=1.0f);

	private:		
		Mesh*		m_Mesh;
		float		m_Scale;
	};

	//////////////////////////////////////////////////////////////////////////
	class MeshRenderer : public RenderComponent
	{
	public:
		//////////////////////////////////////////////////////////////////////////
		// Override IComponent functions
		//////////////////////////////////////////////////////////////////////////
		int GetTypeID() const { return s_TypeID; }

		static uint32_t s_TypeID;

		void LoadFromXML( tinyxml2::XMLElement* data );

		IComponent* MakeCopyDerived() const;
		void Init();

		//////////////////////////////////////////////////////////////////////////
		// Override RenderComponent functions
		//////////////////////////////////////////////////////////////////////////
		void AddToRenderQueue(RenderQueue& renderQueue, const std::vector<Light*>& lightList);

		static void PbrRenderFunc( MeshRenderer* renderer, DrawCallData* drawCallData );
		static void BarrierFunc( MeshRenderer* renderer, DrawCallData* drawCallData );
		static void DefaultRenderFunc( MeshRenderer* renderer, DrawCallData* drawCallData );
		static void PbrFlowFunc(MeshRenderer* renderer, DrawCallData* drawCallData);
		Aabb GetRenderAabb() const;

		//////////////////////////////////////////////////////////////////////////
		// MeshRenderer functions
		//////////////////////////////////////////////////////////////////////////
		void SetMesh(Mesh* mesh);

		void SetMaterial(Material* mat);

	protected:
		void       (*m_RenderFunc)(MeshRenderer*, DrawCallData*);
		Mesh*		m_Mesh;
		Material*	m_Material;
		MaterialRenderType	m_RenderType;
		uint32_t	m_Passes[3];
		uint32_t	m_PassCount;
	};

	//////////////////////////////////////////////////////////////////////////
	class SkinnedMeshRenderer : public MeshRenderer
	{
	public:
		//////////////////////////////////////////////////////////////////////////
		// Override IComponent functions
		//////////////////////////////////////////////////////////////////////////
		int GetTypeID() const { return s_TypeID; }

		void LoadFromXML(tinyxml2::XMLElement* data);

		static uint32_t s_TypeID;

		IComponent* MakeCopyDerived() const;

		//////////////////////////////////////////////////////////////////////////
		// Override RenderComponent functions
		//////////////////////////////////////////////////////////////////////////
		void AddToRenderQueue( RenderQueue& renderQueue, const std::vector<Light*>& lightList );

		//////////////////////////////////////////////////////////////////////////
		// MeshRenderer functions
		//////////////////////////////////////////////////////////////////////////
		void SetAnimationId( unsigned int id ) { m_AnimationId = id; }

	private:
		uint32_t	m_AnimationId;
		bool m_lightWeight;
	};
}
