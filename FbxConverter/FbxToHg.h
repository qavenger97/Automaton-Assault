#pragma once
#include "pch.h"
#include "Renderer/Mesh.h"
#include "Component/Animation.h"
#include "Structs.h"
#include <unordered_map>

class FbxToHg
{
public:

	bool Initialize();
	bool LoadScene(const char* inFileName, const char* inOutputPath);	
	ConvertedData FbxAnimToHgAnim();
	ConvertedData FbxMeshToHgMesh();
	void WriteBoneIdToFile(const char* path) const;
	void Shutdown();

private:

	struct FbxData
	{
		FbxManager* manager;
		FbxScene*	scene;
		FbxNode*	root;
		float		scale_factor;
	} m_Fbx;

	struct FatData
	{
		std::vector<FatTriangle> triangles;
		std::vector<FatVert> vertices;
		FatSkeleton skeleton;
	} m_Fat;

	struct AnimData
	{
		float		length;
		std::string name;
	} m_AnimData;	
	
	std::unordered_map<unsigned int, VertexPosInfl> m_MapFbxIdToPosInfls;
	std::unordered_map<std::string, int> m_MapNameToJointId;

private:
	void ExtractCpAndAnim( FbxNode* node );
	void ExtractCpAndMesh( FbxNode* node );
	void ExtractHierarchy(FbxNode* node );
	void ExtractHierarchy(FbxNode* node, int parentId);
	void ProcessControlPoints(FbxNode* node);
	void ExtractSkinData( FbxNode* node );
	void ExtractKeyframes( FbxNode* node );
	void ProcessMesh(FbxNode* node);
	void ExtractUV(FbxMesh* inMesh, int inCtrlPointIndex, int inTextureUVIndex, int inUVLayer, XMFLOAT2& outUV);
	void ExtractNormal(FbxMesh* inMesh, int inCtrlPointIndex, int inVertexCounter, XMFLOAT3& outNormal);
	void ExtractBinormal(FbxMesh* inMesh, int inCtrlPointIndex, int inVertexCounter, XMFLOAT3& outBinormal);
	void ExtractTangent(FbxMesh* inMesh, int inCtrlPointIndex, int inVertexCounter, XMFLOAT4& outTangent);
	void Optimize();
	int FindVertex(const FatVert& inTargetVertex, const std::vector<FatVert>& uniqueVertices);
	
	

	void CreateConvertedMesh(Hourglass::Mesh& mesh);
	void CreateConvertedSkeleton(Hourglass::LoadingSkeleton& skeleton);
	void CreateConvertedClip(Hourglass::Clip& clip);
};