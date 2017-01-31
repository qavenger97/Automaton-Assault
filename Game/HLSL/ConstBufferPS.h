#ifndef _CONSTBUFFERPS_H
#define _CONSTBUFFERPS_H

#include "SharedDefines.h"

#define MAX_POINT_LIGHT_PER_OBJECT 16
#define LIGHT_TYPE_POINT_LIGHT 0
#define LIGHT_TYPE_SPOTLIGHT 1

struct _regAlign LightInfo
{
	float4	PositionRadius;				// World space position and w component as light radius
	float4  Color;						// w component as light intensity
	float4	ConeRatio;					// x, y as inner/outer cone ratio
	float3	Direction;
	int		Type;
};

CONSTANT_BUFFER_BEGIN(SHADER_LIGHT_BUFFER, b0)
LightInfo	lights[MAX_POINT_LIGHT_PER_OBJECT];
float4		cameraPos;					// TODO: Move this to a global constant buffer
int			lightCount;
int			useNormalMap;
int			useSpecularMap;
int			useEmissiveMap;
float		time;
CONSTANT_BUFFER_END

#endif