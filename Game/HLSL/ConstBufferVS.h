#ifndef _CONSTBUFFERVS_H
#define _CONSTBUFFERVS_H

#include "SharedDefines.h"

CONSTANT_BUFFER_BEGIN(SHADER_SCENE_BUFFER, b0)
float4x4	viewMatrix;
float4x4	viewProjMatrix;
float4		screenSize;					// z for texel size width; w for texel size height
CONSTANT_BUFFER_END

CONSTANT_BUFFER_BEGIN(SHADER_OBJECT_BUFFER, b1)
float4x4	worldMatrix;
float4		uniformColor;
CONSTANT_BUFFER_END

CONSTANT_BUFFER_BEGIN(POSE_BUFFER, b2)
float4x4	jointPoses[96];
CONSTANT_BUFFER_END

#endif