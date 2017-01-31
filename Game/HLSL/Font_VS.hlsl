//=============================================================================
// Font_VS.hlsl by Shiyang Ao, 2016 All Rights Reserved.
//
// 
//=============================================================================

#include "ConstBufferVS.h"

struct INPUT_VERTEX
{
	float3 PosL		: POSITION;
	float4 ColorFg	: COLOR0;
	float4 ColorBg	: COLOR1;
	float2 UV		: TEXCOORD0;
};

struct OUTPUT_VERTEX
{
	float4 PosH		: SV_POSITION;
	float4 ColorFg	: COLOR0;
	float4 ColorBg	: COLOR1;
	float2 UV		: TEXCOORD0;
};

OUTPUT_VERTEX main(INPUT_VERTEX Input)
{
	OUTPUT_VERTEX Out = (OUTPUT_VERTEX)0;

	Out.PosH = (float4)0;
	Out.PosH.x = Input.PosL.x * 2 / screenSize.x - 1;
	Out.PosH.y = -(Input.PosL.y * 2 / screenSize.y - 1);
	Out.PosH.w = 1.0f;
	Out.UV = Input.UV;
	Out.ColorFg = Input.ColorFg;
	Out.ColorBg = Input.ColorBg;

	return Out;
}