
#include "pch.h"

#include "Mesh.h"
#include "RenderDevice.h"
#include "Core\FileUtils.h"

Hourglass::Mesh::Mesh()
	: m_AabbScale(1.0f)
{

}

void Hourglass::Mesh::Serialize(FileArchive& archive)
{
	archive.EnsureHeader("HMDL", 4);
	archive.Serialize(m_Type);
	archive.Serialize(PositionArray);
	archive.Serialize(UV0Array);
	archive.Serialize(NormalArray);
	archive.Serialize(TangentArray);
	archive.Serialize(BoneIdArray);
	archive.Serialize(BoneWeightArray);
	archive.Serialize(TriangleIndices);
	archive.Serialize(Submeshes);	
	archive.Serialize(skeleton);
}

void Hourglass::Mesh::AppendVertices(const std::vector<MeshLoaderVertex>& vertexData, int vertexComponentMask)
{
	std::vector<MeshLoaderVertex>::const_iterator iter;
	for (iter = vertexData.begin(); iter != vertexData.end(); iter++)
	{
		PositionArray.push_back(iter->pos);
		UV0Array.push_back(iter->uv0);
		NormalArray.push_back(iter->normal);
		TangentArray.push_back(iter->tangent);
		BoneIdArray.push_back(iter->boneId);
		BoneWeightArray.push_back(iter->weight);
	}
}

void Hourglass::Mesh::AppendTriangles(const std::vector<UINT>& triangleData, int baseVertexOffset)
{
	Submeshes.push_back({ (UINT)triangleData.size(), (UINT)TriangleIndices.size(), baseVertexOffset });
	TriangleIndices.insert(TriangleIndices.end(), triangleData.begin(), triangleData.end());
}

void Hourglass::Mesh::UpdateRenderBuffer()
{
	m_RenderBuffer.Reset();

	BYTE* compactVertexData = nullptr;
	int offset = 0;

	if (m_Type == Type::kStandard)
	{

		int stride = sizeof( float ) * 12;		// Size of position, UV0, normal and tangent

		compactVertexData = new BYTE[PositionArray.size() * stride];

		for (size_t i = 0; i < PositionArray.size(); i++)
		{
			memcpy( compactVertexData + offset, &PositionArray[i], sizeof( Vector3 ) );
			offset += sizeof( Vector3 );
			memcpy( compactVertexData + offset, &UV0Array[i], sizeof( Vector2 ) );
			offset += sizeof( Vector2 );
			memcpy( compactVertexData + offset, &NormalArray[i], sizeof( Vector3 ) );
			offset += sizeof( Vector3 );
			memcpy( compactVertexData + offset, &TangentArray[i], sizeof( Vector4 ) );
			offset += sizeof( Vector4 );
		}

		m_RenderBuffer.CreateVertexBuffer(	compactVertexData, stride, (UINT)PositionArray.size(),
											D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST,
											g_InputLayouts[kVertexDecl_PosUV0NormTan].Get() );
	}
	else if (m_Type == Type::kSkinned)
	{
		int stride = sizeof( float ) * 16 + sizeof( int ) * 4;		// Size of position, UV0, normal, tangent, bone id and bone weight

		compactVertexData = new BYTE[PositionArray.size() * stride];

		for (size_t i = 0; i < PositionArray.size(); i++)
		{
			memcpy( compactVertexData + offset, &PositionArray[i], sizeof( Vector3 ) );
			offset += sizeof( Vector3 );
			memcpy( compactVertexData + offset, &UV0Array[i], sizeof( Vector2 ) );
			offset += sizeof( Vector2 );
			memcpy( compactVertexData + offset, &NormalArray[i], sizeof( Vector3 ) );
			offset += sizeof( Vector3 );
			memcpy( compactVertexData + offset, &TangentArray[i], sizeof( Vector4 ) );
			offset += sizeof( Vector4 );
			memcpy( compactVertexData + offset, &BoneIdArray[i], sizeof( XMINT4 ) );
			offset += sizeof( XMINT4 );
			memcpy( compactVertexData + offset, &BoneWeightArray[i], sizeof( Vector4 ) );
			offset += sizeof( Vector4 );
		}

		m_RenderBuffer.CreateVertexBuffer( compactVertexData, stride, (UINT)PositionArray.size(),
			D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST,
			g_InputLayouts[kVertexDecl_PosUV0NormTanSkin].Get() );
	}
	else
	{
		assert( 0 );
	}

	int indexCount = 0;
	for (int i = 0; i < Submeshes.size(); i++)
	{
		indexCount += Submeshes[i].indexCount;
	}

	m_RenderBuffer.CreateIndexBuffer(TriangleIndices.data(), sizeof(UINT), indexCount);

	delete[] compactVertexData;

	CalculateAabb();
}

void Hourglass::Mesh::CalculateAabb()
{
	// Invalidate current aabb
	m_Aabb.Invalidate();

	// Create aabb based on vertex positions
	for (std::vector<Vector3>::iterator iter = PositionArray.begin(); iter != PositionArray.end(); iter++)
		m_Aabb.Expand(*iter * m_AabbScale);
}

Hourglass::MeshManager Hourglass::g_MeshManager;

void Hourglass::MeshManager::Init()
{
	LoadMeshes("Assets/Mesh/");
}

void Hourglass::MeshManager::Shutdown()
{
	for (std::vector<Mesh*>::iterator iter = m_Meshes.begin(); iter != m_Meshes.end(); iter++)
	{
		delete *iter;
	}
	m_Meshes.clear();
}

Hourglass::Mesh* Hourglass::MeshManager::GetMesh(const char* path) const
{
	std::string meshPath(path);
	FileUtils::TrimPathDelimiter(meshPath);

	for (int i = 0; i < m_Meshes.size(); i++)
	{
		if (FileUtils::iequals(meshPath, m_Meshes[i]->GetPath()))
			return m_Meshes[i];
	}

	return nullptr;
}

void Hourglass::MeshManager::LoadMeshes(const char* path)
{
	char searchPath[MAX_PATH];
	strcpy_s(searchPath, path);
	strcat_s(searchPath, "\\*.*");

	FileUtils::IterateFolderForFiles(searchPath, ".hmdl", [&](const char* filename) {
		FileArchive archive;

		if (archive.Open(filename, kFileOpenMode_Read))
		{
			Mesh* pMesh = new Mesh;

			// Unify delimiter in path
			std::string strFilename = filename;
			FileUtils::TrimPathDelimiter(strFilename);

			pMesh->SetPath(strFilename);
			pMesh->Serialize(archive);

			// Load xml for extra settings
			std::string xmlFile = strFilename.substr(0, strFilename.length() - 4);
			xmlFile += "xml";
			tinyxml2::XMLDocument doc;
			if (doc.LoadFile(xmlFile.c_str()) == tinyxml2::XML_SUCCESS)
			{
				tinyxml2::XMLElement * xmlMesh = doc.FirstChildElement("Mesh");

				if (xmlMesh)
				{
					float aabbScale = 1.0f;
					xmlMesh->QueryFloatAttribute("AabbScale", &aabbScale);
					pMesh->SetAabbScale(aabbScale);
				}
			}

			pMesh->UpdateRenderBuffer();

			m_Meshes.push_back(pMesh);
			archive.Close();
		}
		else
		{
			char buf[1024];
			sprintf_s(buf, "Unable to load mesh: %s\n", filename);
			OutputDebugStringA(buf);
		}
	});
}
