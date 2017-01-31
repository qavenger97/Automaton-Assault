#include "FileCollection.h"
#include "FbxToHg.h"
#include "Animation\Channel.h"
#include "Component\Animation.h"
#include <iostream>

const Vector4 FbxUtil::epsilon = Vector4( 0.00001f, 0.00001f, 0.00001f, 0.00001f);

bool FbxUtil::IsEqual( float lhs, float rhs )
{
	return fabsf( lhs - rhs ) < FLT_EPSILON;
}

bool FbxUtil::IsEqual(const XMFLOAT4& lhs, const XMFLOAT4& rhs)
{
	return XMVector4NearEqual(XMLoadFloat4(&lhs), XMLoadFloat4(&rhs), epsilon) == TRUE;
}

bool FbxUtil::IsEqual( const XMFLOAT3& lhs, const XMFLOAT3& rhs )
{
	return XMVector3NearEqual( XMLoadFloat3( &lhs ), XMLoadFloat3( &rhs ), epsilon ) == TRUE;
}

bool FbxUtil::IsEqual( const XMFLOAT2& lhs, const XMFLOAT2& rhs )
{
	return XMVector2NearEqual( XMLoadFloat2( &lhs ), XMLoadFloat2( &rhs ), epsilon ) == TRUE;
}

FbxAMatrix FbxUtil::ObjOffsetTransform(FbxNode* node)
{
	return FbxAMatrix(  node->GetGeometricTranslation( FbxNode::eSourcePivot ),
						node->GetGeometricRotation(FbxNode::eSourcePivot),
						node->GetGeometricScaling(FbxNode::eSourcePivot) );
}

void FbxUtil::Convert( const char* path, FbxType type )
{
	FileCollection fc;
	fc.AddFilesInDir( path, "fbx", FileCollection::kIncludeSubDirs );

	unsigned int count = fc.Count();

	std::cout << fc.Count() << " fbx files have been found in directory: " << path << std::endl;

	for (unsigned int i = 0; i < count; ++i)
	{
		FileProperties fp = fc.Get( i );

		FbxToHg fbxToHg;
		fbxToHg.Initialize();

		fbxToHg.LoadScene( fp.GetPath(), fp.GetPathNoExt() );

		if (type == FbxType::kAnimation)
		{
			std::cout << "Extracting fbx mesh data (file " << i << " of " << count << ") @ " << fp.GetPath() << std::endl << std::endl;
			ConvertedData data = fbxToHg.FbxAnimToHgAnim();
			hg::FileArchive archive;
			std::string outputName = std::string( fp.GetPathNoExt() ).append( ".hani" );

			std::cout << "Data from " << fp.GetPath() << " has been extracted, now writing to file..." << std::endl;

			archive.Open( outputName.c_str(), hg::kFileOpenMode_Write );
			archive.Serialize( data.m_Skeleton );
			archive.Serialize( data.m_AnimLength );
			data.m_Clip.Serialize( &archive );
			archive.Close();

			std::cout << outputName << " has been created." << std::endl << std::endl << std::endl;
		}
		else if (type == FbxType::kMesh)
		{
			std::cout << "Extracting fbx mesh data (file " << i << " of " << count <<  ") @ " << fp.GetPath() << std::endl;
			ConvertedData data = fbxToHg.FbxMeshToHgMesh();

			char bondIdFile[1024];
			strcpy_s(bondIdFile, 1024, fp.GetPathNoExt());
			strcat_s(bondIdFile, 1024, ".txt");
			fbxToHg.WriteBoneIdToFile(bondIdFile);

			hg::FileArchive archive;
			std::string outputName = std::string( fp.GetPathNoExt() ).append( ".hmdl" );

			std::cout << "Data from " << fp.GetPath() << " has been extracted, now writing to file..." << std::endl;

			archive.Open( outputName.c_str(), hg::kFileOpenMode_Write );
			data.m_Mesh.Serialize( archive );
			archive.Close();

			std::cout << outputName << " has been created." << std::endl << std::endl << std::endl;
		}
	}
}

void FbxUtil::GlobalTween( const hg::Channel& channel, hg::Transform::TransformData& tween, float currentTime )
{
	unsigned int keyCount = channel.GetChannelCount();
	const float* keyTimes = channel.GetChannelKeyTimes();
	const hg::KeyFrame* keyFrames = channel.GetChannelKeyFrames();

	for (unsigned int kf = 1; kf < keyCount; ++kf)
	{
		if (currentTime < keyTimes[kf])
		{
			const hg::KeyFrame& front = keyFrames[kf];
			const hg::KeyFrame& back = keyFrames[kf - 1];

			float frontTime = keyTimes[kf];
			float backTime = keyTimes[kf - 1];

			float frameTime = currentTime - backTime;
			float tweenTime = frontTime - backTime;
			float dt = frameTime / tweenTime;

			tween.rotation = Quaternion::Lerp( back.m_Global.rotation, front.m_Global.rotation, dt );
			tween.position =  Vector3::Lerp( back.m_Global.position, front.m_Global.position, dt );
			tween.scale = Vector3::Lerp( back.m_Global.scale, front.m_Global.scale, dt );

			break;
		}
	}
}

