#include "FbxToHg.h"
#include "FbxUtil.h"
#include <fstream>
#include <sstream>
#include <iomanip>

bool FbxToHg::Initialize()
{	
	m_Fbx.manager = FbxManager::Create();

	if (!m_Fbx.manager)
	{
		return false;
	}

	FbxIOSettings* fbxIOSettings = FbxIOSettings::Create(m_Fbx.manager, IOSROOT);
	m_Fbx.manager->SetIOSettings(fbxIOSettings);

	m_Fbx.scene = FbxScene::Create(m_Fbx.manager, "theScene");

	return true;
}

bool FbxToHg::LoadScene(const char* inFileName, const char* inOutputPath)
{
	FbxImporter* fbxImporter = FbxImporter::Create(m_Fbx.manager, "myImporter");

	if (!fbxImporter)
	{
		return false;
	}

	if (!fbxImporter->Initialize(inFileName, -1, m_Fbx.manager->GetIOSettings()))
	{
		return false;
	}

	if (!fbxImporter->Import(m_Fbx.scene))
	{
		return false;
	}
	fbxImporter->Destroy();

	FbxGeometryConverter lGeomConverter( m_Fbx.manager );

	lGeomConverter.Triangulate( m_Fbx.scene, true, true );

	//FbxSystemUnit::m.ConvertScene(m_Fbx.scene);

	FbxSystemUnit fbxUnit = m_Fbx.scene->GetGlobalSettings().GetSystemUnit();
	m_Fbx.scale_factor = (float)FbxSystemUnit::m.GetConversionFactorFrom(fbxUnit);
	std::cout << "Convert factor: " << m_Fbx.scale_factor << std::endl;
	std::cout << "Scale factor: " << fbxUnit.GetScaleFactor() << std::endl;

	m_Fbx.root = m_Fbx.scene->GetRootNode();

	return true;
}

ConvertedData FbxToHg::FbxAnimToHgAnim()
{

	ExtractHierarchy( m_Fbx.root );

	if (m_Fat.skeleton.mJoints.empty())
	{
		return ConvertedData();
	}

	ExtractCpAndAnim( m_Fbx.root );

	ConvertedData data;
	
	data.m_AnimLength = (float)m_AnimData.length;
	CreateConvertedSkeleton( data.m_Skeleton );
	CreateConvertedClip( data.m_Clip );

	Shutdown();

	return data;
}

ConvertedData FbxToHg::FbxMeshToHgMesh()
{
	ExtractHierarchy( m_Fbx.root );
	ExtractCpAndMesh( m_Fbx.root );

	ConvertedData data;
	CreateConvertedMesh( data.m_Mesh );

	Shutdown();

	return data;
}

void FbxToHg::WriteBoneIdToFile(const char* path) const
{
	if (m_MapNameToJointId.size() == 0)
		return;

	std::ofstream out;
	out.open(path);

	for (auto i = m_MapNameToJointId.begin(); i != m_MapNameToJointId.end(); i++)
	{
		out << i->first.c_str() << " : " << i->second << std::endl;
	}

	out.close();
}

void FbxToHg::ExtractCpAndAnim(FbxNode* node)
{
	FbxNodeAttribute* attr = node->GetNodeAttribute();
	
	if (attr && attr->GetAttributeType() == FbxNodeAttribute::eMesh)
	{
		ProcessControlPoints( node );
		ExtractKeyframes( node );
	}

	for (int i = 0; i < node->GetChildCount(); ++i)
	{
		ExtractCpAndAnim( node->GetChild( i ) );
	}
}

void FbxToHg::ExtractCpAndMesh( FbxNode* node )
{
	FbxNodeAttribute* attr = node->GetNodeAttribute();

	if (attr && attr->GetAttributeType() == FbxNodeAttribute::eMesh)
	{
		ProcessControlPoints( node );
		ExtractSkinData( node );
		ProcessMesh( node );
	}

	for (int i = 0; i < node->GetChildCount(); ++i)
	{
		ExtractCpAndMesh( node->GetChild( i ) );
	}
}

void FbxToHg::ExtractHierarchy(FbxNode* node)
{
	int count = int(node->GetChildCount());
	for (int childIndex = 0; childIndex < count; ++childIndex)
	{
		FbxNode* currNode = node->GetChild(childIndex);
		ExtractHierarchy(currNode, -1);
	}
}

void FbxToHg::ExtractHierarchy(FbxNode* node, int parentId)
{
	int currId = int(m_Fat.skeleton.mJoints.size());
	FbxNodeAttribute* attr = node->GetNodeAttribute();
	if(attr && attr->GetAttributeType() == FbxNodeAttribute::eSkeleton)
	{
		FatJoint currJoint;
		currJoint.mParentIndex = parentId;
		currJoint.mName = node->GetName();
		currJoint.mNode = node;
		m_MapNameToJointId[currJoint.mName] = currId;
		m_Fat.skeleton.mJoints.push_back(currJoint);
	}
	for (int i = 0; i < node->GetChildCount(); i++)
	{
		ExtractHierarchy(node->GetChild(i), currId);
	}
}

void FbxToHg::ProcessControlPoints(FbxNode* node)
{
	FbxMesh* mesh = node->GetMesh();
	unsigned int ctrlPointCount = mesh->GetControlPointsCount();
	for(unsigned int i = 0; i < ctrlPointCount; ++i)
	{
		VertexPosInfl* posInfl = &m_MapFbxIdToPosInfls[i];
		posInfl->m_Pos = Vector3(	float( mesh->GetControlPointAt( i ).mData[0] ), 
									float( mesh->GetControlPointAt( i ).mData[1] ), 
									float( mesh->GetControlPointAt( i ).mData[2] ) );
	}
}

void FbxToHg::ExtractSkinData( FbxNode* node )
{
	FbxMesh* mesh = node->GetMesh();
	unsigned int dcount = mesh->GetDeformerCount();

	FbxAMatrix objOffsetTransform = FbxUtil::ObjOffsetTransform( node );

	for (unsigned int defIt = 0; defIt < dcount; ++defIt)
	{
		FbxSkin* skin = (FbxSkin*)(mesh->GetDeformer( defIt, FbxDeformer::eSkin ));
		if (!skin)
		{
			continue;
		}

		unsigned int ccount = skin->GetClusterCount();
		for (unsigned int clusterIndex = 0; clusterIndex < ccount; ++clusterIndex)
		{
			FbxCluster* cluster = skin->GetCluster( clusterIndex );
			std::string jointName = cluster->GetLink()->GetName();
			unsigned int jointId = m_MapNameToJointId[jointName];

			unsigned int icount = cluster->GetControlPointIndicesCount();
			for (unsigned int indIt = 0; indIt < icount; ++indIt)
			{
				FatBoneInfl currBlendingIndexWeightPair;
				currBlendingIndexWeightPair.m_Index = jointId;
				currBlendingIndexWeightPair.m_Weight = cluster->GetControlPointWeights()[indIt];
				m_MapFbxIdToPosInfls[cluster->GetControlPointIndices()[indIt]].m_BoneInfluences.push_back( currBlendingIndexWeightPair );
			}
		}
	}

	for (auto itr = m_MapFbxIdToPosInfls.begin(); itr != m_MapFbxIdToPosInfls.end(); ++itr)
	{
		unsigned int bcount = unsigned int(itr->second.m_BoneInfluences.size());
		for (unsigned int i = bcount; i <= 4; ++i)
		{
			itr->second.m_BoneInfluences.push_back( FatBoneInfl() );
		}
	}
}

void FbxToHg::ExtractKeyframes( FbxNode * node )
{
	FbxAnimStack* currAnimStack = m_Fbx.scene->GetSrcObject<FbxAnimStack>( 0 );
	FbxString animStackName = currAnimStack->GetName();
	m_AnimData.name = animStackName.Buffer();
	FbxTakeInfo* takeInfo = m_Fbx.scene->GetTakeInfo( animStackName );
	FbxLongLong bindFrame = takeInfo->mLocalTimeSpan.GetStart().GetFrameCount();
	FbxTime start;
	start.SetFrame( bindFrame + 1 );
	FbxTime end = takeInfo->mLocalTimeSpan.GetStop();
	double startTime = start.GetSecondDouble();
	double endTime = end.GetSecondDouble();
	m_AnimData.length = float( endTime - startTime );
	FbxMesh* mesh = node->GetMesh();
	unsigned int dcount = mesh->GetDeformerCount();

	// since we might deal with obj->fbx files, need to deal with offset if there is one
	FbxAMatrix objOffsetTransform= FbxUtil::ObjOffsetTransform( node );

	for (unsigned int defIt = 0; defIt < dcount; ++defIt)
	{
		FbxSkin* skin = reinterpret_cast<FbxSkin*>(mesh->GetDeformer( defIt, FbxDeformer::eSkin ));
		if (!skin)
		{
			continue;
		}

		unsigned int ccount = skin->GetClusterCount();
		for (unsigned int clusterIndex = 0; clusterIndex < ccount; ++clusterIndex)
		{
			FbxCluster* cluster = skin->GetCluster( clusterIndex );
			std::string jointName = cluster->GetLink()->GetName();
			unsigned int currJointIndex = m_MapNameToJointId[jointName];
			FbxAMatrix transformMatrix;
			FbxAMatrix transformLinkMatrix;
			FbxAMatrix globalBindposeInverseMatrix;

			cluster->GetTransformMatrix( transformMatrix );	
			cluster->GetTransformLinkMatrix( transformLinkMatrix );	

			globalBindposeInverseMatrix = transformLinkMatrix.Inverse() * transformMatrix * objOffsetTransform;

			m_Fat.skeleton.mJoints[currJointIndex].mGlobalBindposeInverse = globalBindposeInverseMatrix;
			m_Fat.skeleton.mJoints[currJointIndex].mNode = cluster->GetLink();

			for (FbxLongLong i = start.GetFrameCount( FbxTime::eFrames24 ); i <= end.GetFrameCount( FbxTime::eFrames24 ); ++i)
			{
				Keyframe kf;
				FbxTime currTime;
				currTime.SetFrame( i, FbxTime::eFrames24 );
				kf.mFrameTime = (float)currTime.GetSecondDouble();
				FbxAMatrix currentTransformOffset = node->EvaluateGlobalTransform( currTime ) * objOffsetTransform;
 				kf.mGlobalTransform = currentTransformOffset.Inverse() * cluster->GetLink()->EvaluateGlobalTransform( currTime );
				m_Fat.skeleton.mJoints[currJointIndex].mAnimation.push_back( kf );
			}
		}
	}
}

void FbxToHg::ProcessMesh(FbxNode* node)
{
	FbxMesh* mesh = node->GetMesh();

	unsigned int triCount = mesh->GetPolygonCount();
	int vertexCounter = 0;
	int vertexStart = (int)m_Fat.vertices.size();
	m_Fat.triangles.reserve(triCount);

	for (unsigned int i = 0; i < triCount; ++i)
	{
		XMFLOAT3 normal[3];
		XMFLOAT4 tangent[3];
		XMFLOAT3 binormal[3];
		XMFLOAT2 UV[3][2];
		FatTriangle currTriangle;
		m_Fat.triangles.push_back(currTriangle);

		for (unsigned int j = 0; j < 3; ++j)
		{
			int ctrlPointIndex = mesh->GetPolygonVertex(i, j);
			VertexPosInfl& posInfl = m_MapFbxIdToPosInfls[ctrlPointIndex];

			ExtractNormal(mesh, ctrlPointIndex, vertexCounter, normal[j]);
			ExtractTangent( mesh, ctrlPointIndex, vertexCounter, tangent[j] );
			ExtractBinormal( mesh, ctrlPointIndex, vertexCounter, binormal[j] );

			ExtractUV( mesh, ctrlPointIndex, mesh->GetTextureUVIndex( i, j ), 0, UV[j][0] );

			FatVert temp;
			temp.mPosition = posInfl.m_Pos;
			temp.mNormal = normal[j];
			temp.mTangent = tangent[j];
			temp.mBinormal = binormal[j];
			temp.mUV0 = UV[j][0];
			temp.mUV1 = UV[j][1];
			
			for(unsigned int i = 0; i < posInfl.m_BoneInfluences.size(); ++i)
			{
				FatBoneInfl currBlendingInfo;
				currBlendingInfo.m_Index = posInfl.m_BoneInfluences[i].m_Index;
				currBlendingInfo.m_Weight = posInfl.m_BoneInfluences[i].m_Weight;
				temp.m_FatBoneInfluences.push_back(currBlendingInfo);
			}

			temp.SortWeights();

			m_Fat.vertices.push_back(temp);
			m_Fat.triangles.back().mIndices.push_back(vertexStart + vertexCounter);
			++vertexCounter;
		}
	}

	m_MapFbxIdToPosInfls.clear();
}

void FbxToHg::ExtractUV(FbxMesh* inMesh, int inCtrlPointIndex, int inTextureUVIndex, int inUVLayer, XMFLOAT2& outUV)
{
	if(inUVLayer >= 2 || inMesh->GetElementUVCount() <= inUVLayer)
	{
		outUV = XMFLOAT2( 0.0f, 0.0f );
		return;
	}
	FbxGeometryElementUV* vertexUV = inMesh->GetElementUV(inUVLayer);

	switch(vertexUV->GetMappingMode())
	{
	case FbxGeometryElement::eByControlPoint:
		switch(vertexUV->GetReferenceMode())
		{
		case FbxGeometryElement::eDirect:
		{
			outUV.x = static_cast<float>(vertexUV->GetDirectArray().GetAt(inCtrlPointIndex).mData[0]);
			outUV.y = static_cast<float>(vertexUV->GetDirectArray().GetAt(inCtrlPointIndex).mData[1]);
		}
		break;

		case FbxGeometryElement::eIndexToDirect:
		{
			int index = vertexUV->GetIndexArray().GetAt(inCtrlPointIndex);
			outUV.x = static_cast<float>(vertexUV->GetDirectArray().GetAt(index).mData[0]);
			outUV.y = static_cast<float>(vertexUV->GetDirectArray().GetAt(index).mData[1]);
		}
		break;

		default:
			throw std::exception("Invalid Reference");
		}
		break;

	case FbxGeometryElement::eByPolygonVertex:
		switch(vertexUV->GetReferenceMode())
		{
		case FbxGeometryElement::eDirect:
		case FbxGeometryElement::eIndexToDirect:
		{
			outUV.x = static_cast<float>(vertexUV->GetDirectArray().GetAt(inTextureUVIndex).mData[0]);
			outUV.y = static_cast<float>(vertexUV->GetDirectArray().GetAt(inTextureUVIndex).mData[1]);
		}
		break;

		default:
			throw std::exception("Invalid Reference");
		}
		break;
	}
}

void FbxToHg::ExtractNormal(FbxMesh* inMesh, int inCtrlPointIndex, int inVertexCounter, XMFLOAT3& outNormal)
{
	if(inMesh->GetElementNormalCount() < 1)
	{
		throw std::exception("Invalid Normal Number");
	}

	FbxGeometryElementNormal* vertexNormal = inMesh->GetElementNormal(0);
	switch(vertexNormal->GetMappingMode())
	{
	case FbxGeometryElement::eByControlPoint:
		switch(vertexNormal->GetReferenceMode())
		{
		case FbxGeometryElement::eDirect:
		{
			outNormal.x = static_cast<float>(vertexNormal->GetDirectArray().GetAt(inCtrlPointIndex).mData[0]);
			outNormal.y = static_cast<float>(vertexNormal->GetDirectArray().GetAt(inCtrlPointIndex).mData[1]);
			outNormal.z = static_cast<float>(vertexNormal->GetDirectArray().GetAt(inCtrlPointIndex).mData[2]);
		}
		break;

		case FbxGeometryElement::eIndexToDirect:
		{
			int index = vertexNormal->GetIndexArray().GetAt(inCtrlPointIndex);
			outNormal.x = static_cast<float>(vertexNormal->GetDirectArray().GetAt(index).mData[0]);
			outNormal.y = static_cast<float>(vertexNormal->GetDirectArray().GetAt(index).mData[1]);
			outNormal.z = static_cast<float>(vertexNormal->GetDirectArray().GetAt(index).mData[2]);
		}
		break;

		default:
			throw std::exception("Invalid Reference");
		}
		break;

	case FbxGeometryElement::eByPolygonVertex:
		switch(vertexNormal->GetReferenceMode())
		{
		case FbxGeometryElement::eDirect:
		{
			outNormal.x = static_cast<float>(vertexNormal->GetDirectArray().GetAt(inVertexCounter).mData[0]);
			outNormal.y = static_cast<float>(vertexNormal->GetDirectArray().GetAt(inVertexCounter).mData[1]);
			outNormal.z = static_cast<float>(vertexNormal->GetDirectArray().GetAt(inVertexCounter).mData[2]);
		}
		break;

		case FbxGeometryElement::eIndexToDirect:
		{
			int index = vertexNormal->GetIndexArray().GetAt(inVertexCounter);
			outNormal.x = static_cast<float>(vertexNormal->GetDirectArray().GetAt(index).mData[0]);
			outNormal.y = static_cast<float>(vertexNormal->GetDirectArray().GetAt(index).mData[1]);
			outNormal.z = static_cast<float>(vertexNormal->GetDirectArray().GetAt(index).mData[2]);
		}
		break;

		default:
			throw std::exception("Invalid Reference");
		}
		break;
	}
}

void FbxToHg::ExtractBinormal(FbxMesh* inMesh, int inCtrlPointIndex, int inVertexCounter, XMFLOAT3& outBinormal)
{
	if(inMesh->GetElementBinormalCount() < 1)
	{
		outBinormal = XMFLOAT3( 0.0f, 0.0f, 0.0f );
		return;
	}

	FbxGeometryElementBinormal* vertexBinormal = inMesh->GetElementBinormal(0);
	switch(vertexBinormal->GetMappingMode())
	{
	case FbxGeometryElement::eByControlPoint:
		switch(vertexBinormal->GetReferenceMode())
		{
		case FbxGeometryElement::eDirect:
		{
			outBinormal.x = static_cast<float>(vertexBinormal->GetDirectArray().GetAt(inCtrlPointIndex).mData[0]);
			outBinormal.y = static_cast<float>(vertexBinormal->GetDirectArray().GetAt(inCtrlPointIndex).mData[1]);
			outBinormal.z = static_cast<float>(vertexBinormal->GetDirectArray().GetAt(inCtrlPointIndex).mData[2]);
		}
		break;

		case FbxGeometryElement::eIndexToDirect:
		{
			int index = vertexBinormal->GetIndexArray().GetAt(inCtrlPointIndex);
			outBinormal.x = static_cast<float>(vertexBinormal->GetDirectArray().GetAt(index).mData[0]);
			outBinormal.y = static_cast<float>(vertexBinormal->GetDirectArray().GetAt(index).mData[1]);
			outBinormal.z = static_cast<float>(vertexBinormal->GetDirectArray().GetAt(index).mData[2]);
		}
		break;

		default:
			throw std::exception("Invalid Reference");
		}
		break;

	case FbxGeometryElement::eByPolygonVertex:
		switch(vertexBinormal->GetReferenceMode())
		{
		case FbxGeometryElement::eDirect:
		{
			outBinormal.x = static_cast<float>(vertexBinormal->GetDirectArray().GetAt(inVertexCounter).mData[0]);
			outBinormal.y = static_cast<float>(vertexBinormal->GetDirectArray().GetAt(inVertexCounter).mData[1]);
			outBinormal.z = static_cast<float>(vertexBinormal->GetDirectArray().GetAt(inVertexCounter).mData[2]);
		}
		break;

		case FbxGeometryElement::eIndexToDirect:
		{
			int index = vertexBinormal->GetIndexArray().GetAt(inVertexCounter);
			outBinormal.x = static_cast<float>(vertexBinormal->GetDirectArray().GetAt(index).mData[0]);
			outBinormal.y = static_cast<float>(vertexBinormal->GetDirectArray().GetAt(index).mData[1]);
			outBinormal.z = static_cast<float>(vertexBinormal->GetDirectArray().GetAt(index).mData[2]);
		}
		break;

		default:
			throw std::exception("Invalid Reference");
		}
		break;
	}
}

void FbxToHg::ExtractTangent(FbxMesh* inMesh, int inCtrlPointIndex, int inVertexCounter, XMFLOAT4& outTangent)
{
	// Try generating tangent on this mesh
	if (inMesh->GetElementTangentCount() < 1)
	{
		inMesh->GenerateTangentsDataForAllUVSets(true);
	}

	// if failed to generate tangent (in the case we don't have UV), return zero tangent
	if (inMesh->GetElementTangentCount() < 1)
	{
		outTangent = XMFLOAT4( 0.0f, 0.0f, 0.0f, 0.0f );
		return;
	}

	FbxGeometryElementTangent* vertexTangent = inMesh->GetElementTangent(0);
	switch(vertexTangent->GetMappingMode())
	{
	case FbxGeometryElement::eByControlPoint:
		switch(vertexTangent->GetReferenceMode())
		{
		case FbxGeometryElement::eDirect:
		{
			outTangent.x = static_cast<float>(vertexTangent->GetDirectArray().GetAt(inCtrlPointIndex).mData[0]);
			outTangent.y = static_cast<float>(vertexTangent->GetDirectArray().GetAt(inCtrlPointIndex).mData[1]);
			outTangent.z = static_cast<float>(vertexTangent->GetDirectArray().GetAt(inCtrlPointIndex).mData[2]);
			outTangent.w = static_cast<float>(vertexTangent->GetDirectArray().GetAt(inCtrlPointIndex).mData[3]);
		}
		break;

		case FbxGeometryElement::eIndexToDirect:
		{
			int index = vertexTangent->GetIndexArray().GetAt(inCtrlPointIndex);
			outTangent.x = static_cast<float>(vertexTangent->GetDirectArray().GetAt(index).mData[0]);
			outTangent.y = static_cast<float>(vertexTangent->GetDirectArray().GetAt(index).mData[1]);
			outTangent.z = static_cast<float>(vertexTangent->GetDirectArray().GetAt(index).mData[2]);
			outTangent.w = static_cast<float>(vertexTangent->GetDirectArray().GetAt(index).mData[3]);
		}
		break;

		default:
			throw std::exception("Invalid Reference");
		}
		break;

	case FbxGeometryElement::eByPolygonVertex:
		switch(vertexTangent->GetReferenceMode())
		{
		case FbxGeometryElement::eDirect:
		{
			outTangent.x = static_cast<float>(vertexTangent->GetDirectArray().GetAt(inVertexCounter).mData[0]);
			outTangent.y = static_cast<float>(vertexTangent->GetDirectArray().GetAt(inVertexCounter).mData[1]);
			outTangent.z = static_cast<float>(vertexTangent->GetDirectArray().GetAt(inVertexCounter).mData[2]);
			outTangent.w = static_cast<float>(vertexTangent->GetDirectArray().GetAt(inVertexCounter).mData[3]);
		}
		break;

		case FbxGeometryElement::eIndexToDirect:
		{
			int index = vertexTangent->GetIndexArray().GetAt(inVertexCounter);
			outTangent.x = static_cast<float>(vertexTangent->GetDirectArray().GetAt(index).mData[0]);
			outTangent.y = static_cast<float>(vertexTangent->GetDirectArray().GetAt(index).mData[1]);
			outTangent.z = static_cast<float>(vertexTangent->GetDirectArray().GetAt(index).mData[2]);
			outTangent.w = static_cast<float>(vertexTangent->GetDirectArray().GetAt(index).mData[3]);
		}
		break;

		default:
			throw std::exception("Invalid Reference");
		}
		break;
	}
}

void FbxToHg::Optimize()
{
	char buf[1024];

	sprintf_s(buf, "Vertex count: %zd\n", m_Fat.triangles.size() * 3);
	OutputDebugStringA(buf);

	int count = 0;

	std::vector<FatVert> uniqueVertices;
	for(unsigned int i = 0; i < m_Fat.triangles.size(); ++i)
	{
		for(unsigned int j = 0; j < 3; ++j)
		{
			if(FindVertex(m_Fat.vertices[i * 3 + j], uniqueVertices) == -1)
			{
				uniqueVertices.push_back(m_Fat.vertices[i * 3 + j]);
			}

			count++;

			if (count % 100 == 0)
			{
				sprintf_s(buf, "%d...\n", count);
				OutputDebugStringA(buf);
			}
		}
	}
	for(unsigned int i = 0; i < m_Fat.triangles.size(); ++i)
	{
		for(unsigned int j = 0; j < 3; ++j)
		{
			m_Fat.triangles[i].mIndices[j] = FindVertex(m_Fat.vertices[i * 3 + j], uniqueVertices);
			sprintf_s(buf, "idx: %d\n", m_Fat.triangles[i].mIndices[j]);
			OutputDebugStringA(buf);
		}
	}
	
	m_Fat.vertices.clear();
	m_Fat.vertices = uniqueVertices;
	uniqueVertices.clear();
	std::sort(m_Fat.triangles.begin(), m_Fat.triangles.end());
}

int FbxToHg::FindVertex(const FatVert& inTargetVertex, const std::vector<FatVert>& uniqueVertices)
{
	for(unsigned int i = 0; i < uniqueVertices.size(); ++i)
	{
		if(inTargetVertex == uniqueVertices[i])
		{
			return i;
		}
	}

	return -1;
}

void FbxToHg::Shutdown()
{
	m_Fbx.scene->Destroy();
	m_Fbx.manager->Destroy();

	m_Fat.triangles.clear();

	m_Fat.vertices.clear();

	m_Fat.skeleton.mJoints.clear();
}

void FbxToHg::CreateConvertedMesh(Hourglass::Mesh& mesh)
{
	bool isSkinnedMesh = !m_Fat.skeleton.mJoints.empty();

	if (isSkinnedMesh)
		mesh.SetType( Hourglass::Mesh::Type::kSkinned );
	else
		mesh.SetType( Hourglass::Mesh::Type::kStandard);

	std::vector<unsigned int> outIndices;

	unsigned int triCount = unsigned int(m_Fat.triangles.size());
	outIndices.reserve( triCount );
	for (unsigned int i = 0; i < triCount; ++i)
	{
		outIndices.push_back( m_Fat.triangles[i].mIndices[0] );
		outIndices.push_back( m_Fat.triangles[i].mIndices[2] );
		outIndices.push_back( m_Fat.triangles[i].mIndices[1] );
	}	

	std::vector<Hourglass::MeshLoaderVertex> outVertices;
	for (unsigned int i = 0; i < m_Fat.vertices.size(); ++i)
	{
		FatVert& bv = m_Fat.vertices[i];
		Hourglass::MeshLoaderVertex v;

		if (isSkinnedMesh)
		{
			v.pos =			Vector3( bv.mPosition.x,	bv.mPosition.y, -bv.mPosition.z);
			v.normal =		Vector3( bv.mNormal.x,		bv.mNormal.y,	-bv.mNormal.z	);
			v.tangent =		Vector4( bv.mTangent.x,		bv.mTangent.y,	-bv.mTangent.z,  bv.mTangent.w );
			v.binormal =	Vector3( bv.mBinormal.x,	bv.mBinormal.y,	-bv.mBinormal.z	);
		}
		else
		{
			v.pos =			Vector3( -bv.mPosition.x,	bv.mPosition.y, bv.mPosition.z);
			v.normal =		Vector3( -bv.mNormal.x,		bv.mNormal.y,	bv.mNormal.z	);
			v.tangent =		Vector4( -bv.mTangent.x,	bv.mTangent.y,	bv.mTangent.z,  bv.mTangent.w );
			v.binormal =	Vector3( -bv.mBinormal.x,	bv.mBinormal.y,	bv.mBinormal.z	);

			v.pos *= m_Fbx.scale_factor;
		}

		if (FLT_EQUALS_ZERO(v.tangent.w))
			v.tangent.w = 1.0f;

		v.uv0 =	Vector2( bv.mUV0.x,	1.0f - bv.mUV0.y );
		v.uv1 =	Vector2( bv.mUV1.x,	1.0f - bv.mUV1.y );
		
		if (isSkinnedMesh)
		{
			v.boneId.x = bv.m_FatBoneInfluences[0].m_Index;
			v.boneId.y = bv.m_FatBoneInfluences[1].m_Index;
			v.boneId.z = bv.m_FatBoneInfluences[2].m_Index;
			v.boneId.w = bv.m_FatBoneInfluences[3].m_Index;

			v.weight.x = float(bv.m_FatBoneInfluences[0].m_Weight);
			v.weight.y = float(bv.m_FatBoneInfluences[1].m_Weight);
			v.weight.z = float(bv.m_FatBoneInfluences[2].m_Weight);
			v.weight.w = float(bv.m_FatBoneInfluences[3].m_Weight);
		}

		outVertices.push_back( v );
	}

	char msg_buf[1024];
	sprintf_s(msg_buf, "Optimizing mesh...\n");
	OutputDebugStringA(msg_buf);
	std::map<Hourglass::MeshLoaderVertex, int> meshVertIndexTable;
	std::vector<Hourglass::MeshLoaderVertex> optimizedVertData;
	std::vector<UINT> optimizedIndexData;
	UINT index = 0;
	for (UINT i = 0; i < outIndices.size(); i++)
	{
		Hourglass::MeshLoaderVertex& v = outVertices[outIndices[i]];
		std::map<Hourglass::MeshLoaderVertex, int>::iterator iterResult = meshVertIndexTable.find(v);
		if (iterResult == meshVertIndexTable.end())
		{
			meshVertIndexTable[v] = index;
			optimizedVertData.push_back(v);
			optimizedIndexData.push_back(index);
			index++;
		}
		else
		{
			optimizedIndexData.push_back(iterResult->second);
		}
	}

	mesh.AppendTriangles( optimizedIndexData );
	mesh.AppendVertices( optimizedVertData, 0 );
}

void FbxToHg::CreateConvertedSkeleton(Hourglass::LoadingSkeleton& skeleton)
{	
	for (unsigned int i = 0; i < skeleton.kMaxJoints; ++i)
	{
		skeleton.m_ParentIndices[i] = -1;
	}

	for (unsigned int i = 0; i < m_Fat.skeleton.mJoints.size(); ++i)
	{
		skeleton.m_ParentIndices[i] = m_Fat.skeleton.mJoints[i].mParentIndex;

		FbxVector4 translation = m_Fat.skeleton.mJoints[i].mGlobalBindposeInverse.GetT();
		FbxVector4 rotation = m_Fat.skeleton.mJoints[i].mGlobalBindposeInverse.GetR();
		translation.Set( translation.mData[0], translation.mData[1], -translation.mData[2] );
		rotation.Set( -rotation.mData[0], -rotation.mData[1], rotation.mData[2] );
		m_Fat.skeleton.mJoints[i].mGlobalBindposeInverse.SetT( translation );
		m_Fat.skeleton.mJoints[i].mGlobalBindposeInverse.SetR( rotation );
		FbxMatrix out = m_Fat.skeleton.mJoints[i].mGlobalBindposeInverse;
	
		skeleton.m_BindPoseInverse[i] = Matrix(	(float)out.Get(0,0), (float)out.Get(0,1), (float)out.Get(0,2), (float)out.Get(0,3),
												(float)out.Get(1,0), (float)out.Get(1,1), (float)out.Get(1,2), (float)out.Get(1,3),
												(float)out.Get(2,0), (float)out.Get(2,1), (float)out.Get(2,2), (float)out.Get(2,3),
												(float)out.Get(3,0), (float)out.Get(3,1), (float)out.Get(3,2), (float)out.Get(3,3)	);
	}
}

void FbxToHg::CreateConvertedClip(Hourglass::Clip& clip)
{
	clip.SetName( m_AnimData.name.c_str() );
	hg::Clip globalClip;

	for (unsigned int i = 0; i < m_Fat.skeleton.mJoints.size(); ++i)
	{		
		hg::Channel channel;
		
		if (m_Fat.skeleton.mJoints[i].mAnimation.size() == 0)
		{
			Keyframe kf;
			kf.mFrameTime = 0.0f;
			kf.mGlobalTransform = kf.mGlobalTransform = m_Fat.skeleton.mJoints[i].mNode->EvaluateGlobalTransform( FBXSDK_TIME_INFINITE );
			m_Fat.skeleton.mJoints[i].mAnimation.clear();
			m_Fat.skeleton.mJoints[i].mAnimation.push_back( kf );
			m_Fat.skeleton.mJoints[i].mAnimation.push_back( kf );
		}

		for (auto& kf : m_Fat.skeleton.mJoints[i].mAnimation)
		{
			hg::KeyFrame keyFrame;

			FbxVector4 translation = kf.mGlobalTransform.GetT();
			FbxVector4 rotation = kf.mGlobalTransform.GetR();
			translation.Set( translation.mData[0], translation.mData[1], -translation.mData[2] );
			rotation.Set( -rotation.mData[0], -rotation.mData[1], rotation.mData[2] );
			kf.mGlobalTransform.SetT( translation );
			kf.mGlobalTransform.SetR( rotation );
			FbxMatrix out = kf.mGlobalTransform;

			Matrix mat = Matrix( (float)out.Get( 0, 0 ), (float)out.Get( 0, 1 ), (float)out.Get( 0, 2 ), (float)out.Get( 0, 3 ),
				(float)out.Get( 1, 0 ), (float)out.Get( 1, 1 ), (float)out.Get( 1, 2 ), (float)out.Get( 1, 3 ),
				(float)out.Get( 2, 0 ), (float)out.Get( 2, 1 ), (float)out.Get( 2, 2 ), (float)out.Get( 2, 3 ),
				(float)out.Get( 3, 0 ), (float)out.Get( 3, 1 ), (float)out.Get( 3, 2 ), (float)out.Get( 3, 3 ) );

			Vector3 scale;
			Vector3 pos;
			mat.Decompose( scale, keyFrame.m_Global.rotation, pos );
			keyFrame.m_Global.scale = scale;
			keyFrame.m_Global.position = pos;
			keyFrame.m_Local = keyFrame.m_Global;
			channel.AddFrame( kf.mFrameTime, keyFrame );
		}

		//clip.AddChannel( channel );
		globalClip.AddChannel( channel );
	}
	
	hg::Channel* globalChannels = globalClip.GetChannels();
	unsigned int globalChannelCount = globalClip.GetChannelCount();
	clip.AddChannel(globalChannels[0]);
	unsigned int keyframeCount = globalChannels[0].GetChannelCount();

	for (unsigned int ch = 1; ch < globalChannelCount; ++ch)
	{
		hg::Channel newChannel;
		hg::Channel& currGlobalChannel = globalChannels[ch];
		unsigned int parentId = m_Fat.skeleton.mJoints[ch].mParentIndex;
		hg::Channel& parentGlobalChannel = globalChannels[parentId];
		const float* keytimes = currGlobalChannel.GetChannelKeyTimes();
		const float* parentKeytimes = parentGlobalChannel.GetChannelKeyTimes();
		const hg::KeyFrame* keyframes = currGlobalChannel.GetChannelKeyFrames();
		const hg::KeyFrame* parentKeyframes = parentGlobalChannel.GetChannelKeyFrames();
		unsigned int parentKeyframeCount = parentGlobalChannel.GetChannelCount();
		
		keyframeCount = globalChannels[ch].GetChannelCount();
		for (unsigned int kf = 0; kf < keyframeCount; ++kf)
		{
			hg::KeyFrame newKf;
			const hg::KeyFrame& keyframe = keyframes[kf];			

			if (kf < parentKeyframeCount && FbxUtil::IsEqual( keytimes[kf], parentKeytimes[kf]))
			{
				const hg::KeyFrame& parentKeyframe = parentKeyframes[kf];
				Quaternion parentRotConjug;
				parentKeyframes[kf].m_Global.rotation.Conjugate( parentRotConjug );
				newKf.m_Local.position = Vector3( XMVector3Rotate( (Vector3( keyframes[kf].m_Global.position ) - Vector3( parentKeyframe.m_Global.position )), parentRotConjug ) );
				newKf.m_Local.scale = keyframe.m_Global.scale / parentKeyframe.m_Global.scale;
				newKf.m_Local.rotation = keyframe.m_Global.rotation * parentRotConjug;
			}			
			else if (parentKeyframeCount == 1)
			{
				const hg::KeyFrame& parentKeyframe = parentKeyframes[0];
				Quaternion parentRotConjug;
				parentKeyframes[kf].m_Global.rotation.Conjugate( parentRotConjug );
				newKf.m_Local.position = Vector3( XMVector3Rotate( (Vector3( keyframes[kf].m_Global.position ) - Vector3( parentKeyframe.m_Global.position )), parentRotConjug ) );
				newKf.m_Local.scale = keyframe.m_Global.scale / parentKeyframe.m_Global.scale;
				newKf.m_Local.rotation = keyframe.m_Global.rotation * parentRotConjug;
			}
			else
			{
				hg::KeyFrame tween;
				FbxUtil::GlobalTween( parentGlobalChannel, tween.m_Global, keytimes[kf] );

				Quaternion parentRotConjug;
				tween.m_Global.rotation.Conjugate( parentRotConjug );
				newKf.m_Local.position = Vector3( XMVector3Rotate( (Vector3( keyframes[kf].m_Global.position ) - Vector3( tween.m_Global.position )), parentRotConjug ) );
				newKf.m_Local.scale = keyframe.m_Global.scale / tween.m_Global.scale;
				newKf.m_Local.rotation = keyframe.m_Global.rotation * parentRotConjug;
			}

			
			newKf.m_Local.rotation.Normalize();
			newKf.m_Global = keyframe.m_Global;
			newChannel.AddFrame( keytimes[kf], newKf );
		}

		clip.AddChannel( newChannel );
	}


	/*
	localPos = XMVector3Rotate((pos - parentPos), parentRotConjug) / parentScale;
	localScl = scale / parentScale;
	localRot = XMQuaternionMultiply(rot, parentRotConjug) <-normalize this
	*/
}
