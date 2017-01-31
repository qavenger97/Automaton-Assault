
#include "ConstBufferVS.h"

struct INPUT_VERTEX
{
	float3 PosL		: POSITION;
	float2 UV		: TEXCOORD0;
	float3 Normal	: NORMAL;
	float4 Tangent	: TANGENT;
};

struct OUTPUT_VERTEX
{
	float4 PosH		 : SV_POSITION;
};

OUTPUT_VERTEX main(INPUT_VERTEX Input)
{
	OUTPUT_VERTEX Out = (OUTPUT_VERTEX)0;

	float4 PosW = mul(float4(Input.PosL, 1.0f), worldMatrix);
	Out.PosH = mul(PosW, viewProjMatrix);

	return Out;
}
