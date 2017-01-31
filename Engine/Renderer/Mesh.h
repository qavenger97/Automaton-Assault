#pragma once


#include "RenderBuffer.h"
#include "Core/FileArchive.h"
#include "Math/Aabb.h"
#include "Component/Animation.h"

namespace Hourglass
{
	class DynamicCollider;

	enum VertexComponentType
	{
		kVertexComponent_BoneId,
		kVertexComponent_BoneWeights,
		kVertexComponent_Pos,
		kVertexComponent_UV0,
		kVertexComponent_Normal,
		kVertexComponent_Tangent,
		kVertexComponent_UV1,

		kVertexComponent_Count,
	};

	enum VertexComponentMaskType
	{
		kVCM_BoneId							= 1 << kVertexComponent_BoneId,
		kVCM_BoneWeights					= 1 << kVertexComponent_BoneWeights,
		kVCM_Pos							= 1 << kVertexComponent_Pos,
		kVCM_UV0							= 1 << kVertexComponent_UV0,
		kVCM_Normal							= 1 << kVertexComponent_Normal,
		kVCM_Tangent						= 1 << kVertexComponent_Tangent,
		kVCM_UV1							= 1 << kVertexComponent_UV1,
	};


	struct MeshLoaderVertex
	{
		Vector3 pos;
		Vector2 uv0;
		Vector2 uv1;
		Vector4 tangent;
		Vector3 binormal;
		Vector3 normal;
		XMINT4  boneId;
		Vector4	weight;

		void SetWeight( unsigned int i, float newWeight )
		{
			if (i == 0)		 weight.x = newWeight;
			else if (i == 1) weight.y = newWeight;
			else if (i == 2) weight.z = newWeight;
			else if (i == 3) weight.w = newWeight;
			else assert( false );
		}

		void SetBoneId( unsigned int i, unsigned int newBoneId )
		{
			if (i == 0)		 boneId.x = newBoneId;
			else if (i == 1) boneId.y = newBoneId;
			else if (i == 2) boneId.z = newBoneId;
			else if (i == 3) boneId.w = newBoneId;
			else assert( false );
		}

		// lexicographical compare function for binary sorting
		bool operator<( const MeshLoaderVertex& rhs ) const
		{
			return std::lexicographical_compare(
				(const float*)this,
				(const float*)this + sizeof( MeshLoaderVertex ) / 4,
				(const float*)&rhs,
				(const float*)&rhs + sizeof( MeshLoaderVertex ) / 4 );
		}

	};

	struct SubmeshData
	{
		UINT indexCount;				// Index count for this submesh
		UINT startIndexOffset;			// Starting index in triangle array
		INT  baseVertexOffset;			// Starting vertex index in vertex array
	};

	class Mesh
	{
	public:

		enum Type
		{
			kStandard = 0,
			kSkinned = 1
		};

		Mesh();

		// Serialization
		void Serialize(FileArchive& archive);

		// Append vertices to the mesh
		void AppendVertices(const std::vector<MeshLoaderVertex>& vertexData, int vertexComponentMask);

		// Append triangles to the mesh
		void AppendTriangles(const std::vector<UINT>& triangleData, int baseVertexOffset = 0);

		// Rebuild render buffer with mesh data
		void UpdateRenderBuffer();

		// Get vertex count in the mesh
		int GetVertexCount() const { return (int)PositionArray.size(); }

		Aabb GetAabb() const { return m_Aabb; }

		RenderBuffer* GetRenderBuffer() { return &m_RenderBuffer; }

		void SetPath(const std::string path) { m_Path = path; }
		const std::string& GetPath() const { return m_Path; }

		void SetType( Type type ) { m_Type = type; }

		void SetAabbScale(float scale) { m_AabbScale = scale; }

		std::vector<UINT>			TriangleIndices;
		std::vector<Vector3>		PositionArray;
		std::vector<Vector2>		UV0Array;
		std::vector<Vector3>		NormalArray;
		std::vector<Vector4>		TangentArray;
		//std::vector<Vector2>		UV1Array;
		std::vector<XMINT4>			BoneIdArray;
		std::vector<Vector4>		BoneWeightArray;

		std::vector<SubmeshData>	Submeshes;		

		LoadingSkeleton skeleton;

	private:
		// Calculate aabb based on vertex positions
		void CalculateAabb();

		RenderBuffer				m_RenderBuffer;
		Aabb						m_Aabb;
		float						m_AabbScale;
		std::string					m_Path;
		unsigned int m_Type;
	};

	class MeshManager
	{
	public:
		// Initialize mesh manager
		void Init();

		// Shutdown mesh manager
		void Shutdown();

		// Get a mesh by its path
		Mesh* GetMesh(const char* path) const;

	private:
		// Load meshes from 'Assets/Mesh' folder
		void LoadMeshes(const char* path);

		std::vector<Mesh*>		m_Meshes;
	};

	extern MeshManager g_MeshManager;
}
