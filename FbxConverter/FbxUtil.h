#pragma once
#include "pch.h"
#include <fbxsdk.h>
#include <iostream>
#include <string>
#include "Animation\Channel.h"

#include <fbxsdk.h>

class FbxUtil
{
public:
	enum FbxType
	{
		kAnimation,
		kMesh
	};

	static const Vector4 epsilon;

	static bool IsEqual( float lhs, float rhs );

	static bool IsEqual( const XMFLOAT4& lhs, const XMFLOAT4& rhs );

	static bool IsEqual( const XMFLOAT3& lhs, const XMFLOAT3& rhs );

	static bool IsEqual( const XMFLOAT2& lhs, const XMFLOAT2& rhs );

	static void Convert(const char* path, FbxType type);

	static FbxAMatrix ObjOffsetTransform(FbxNode* node);

	static void GlobalTween( const hg::Channel& channel, hg::Transform::TransformData& tween, float currentTime );
};


