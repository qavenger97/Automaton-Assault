#pragma once
#include "FbxUtil.h"

struct ConvertedData
{
	Hourglass::Mesh		m_Mesh;
	Hourglass::LoadingSkeleton m_Skeleton;
	Hourglass::Clip		m_Clip;
	float				m_AnimLength;
};

struct FatBoneInfl
{
	unsigned int m_Index = 0;
	double m_Weight = 0;

	bool operator < ( const FatBoneInfl& rhs )
	{
		return (m_Weight > rhs.m_Weight);
	}
};

struct VertexPosInfl
{
	XMFLOAT3 m_Pos;
	std::vector<FatBoneInfl> m_BoneInfluences;
};

struct Keyframe
{
	FbxFloat mFrameTime;
	FbxAMatrix mGlobalTransform;
	Keyframe* mNext;

	Keyframe() :
		mNext( nullptr )
	{}
};

struct FatJoint
{
	std::string mName;
	int mParentIndex;
	FbxAMatrix mGlobalBindposeInverse;
	std::vector<Keyframe> mAnimation;
	FbxNode* mNode;

	FatJoint() :
		mNode( nullptr )
	{
		mGlobalBindposeInverse.SetIdentity();
		mParentIndex = -1;
	}
};

struct FatSkeleton
{
	std::vector<FatJoint> mJoints;
};

struct FatTriangle
{
	std::vector<unsigned int> mIndices;
	std::string mMaterialName;
	unsigned int mMaterialIndex;

	bool operator<( const FatTriangle& rhs )
	{
		return mMaterialIndex < rhs.mMaterialIndex;
	}
};

struct FatVert
{
	Vector3 mPosition;
	Vector3 mNormal;
	Vector4 mTangent;
	Vector3 mBinormal;
	Vector2 mUV0;
	Vector2 mUV1;

	std::vector<FatBoneInfl> m_FatBoneInfluences;

	bool operator==( const FatVert& rhs ) const
	{
		if (!FbxUtil::IsEqual( mPosition, rhs.mPosition )) return false;
		if (!FbxUtil::IsEqual( mNormal, rhs.mNormal )) return false;
		if (!FbxUtil::IsEqual( mBinormal, rhs.mBinormal )) return false;
		if (!FbxUtil::IsEqual( mTangent, rhs.mTangent )) return false;
		if (!FbxUtil::IsEqual( mUV0, rhs.mUV0 )) return false;
		if (!FbxUtil::IsEqual( mUV1, rhs.mUV1 )) return false;
		if (m_FatBoneInfluences.empty()) return false;

		unsigned int bcount = unsigned int(m_FatBoneInfluences.size());

		if (bcount!= rhs.m_FatBoneInfluences.size()) return false;

		for (unsigned int i = 0; i < bcount; ++i)
		{
			if (m_FatBoneInfluences[i].m_Index != rhs.m_FatBoneInfluences[i].m_Index) return false;
			if (m_FatBoneInfluences[i].m_Weight - rhs.m_FatBoneInfluences[i].m_Weight > 0.00001) return false;
			if (m_FatBoneInfluences[i].m_Weight - rhs.m_FatBoneInfluences[i].m_Weight < -0.00001) return false;
		}

		return true;
	}

	void SortWeights()
	{
		std::sort( m_FatBoneInfluences.begin(), m_FatBoneInfluences.end() );
	}
	
};
