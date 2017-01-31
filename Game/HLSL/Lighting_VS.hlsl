
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
	float4 Color	 : COLOR;
	float4 PosH		 : SV_POSITION;
	float2 UV		 : TEXCOORD0;
	float3 PosW		 : TEXCOORD1;		// World space position
	float3 NormalW	 : TEXCOORD2;		// World space normal
	float3 TangentW	 : TEXCOORD3;
	float3 BinormalW : TEXCOORD4;
};

OUTPUT_VERTEX main(INPUT_VERTEX Input)
{
	OUTPUT_VERTEX Out = (OUTPUT_VERTEX)0;

	float4 PosW = mul(float4(Input.PosL, 1.0f), worldMatrix);
	Out.PosH = mul(PosW, viewProjMatrix);

	float3 Normal = mul(Input.Normal, (float3x3)worldMatrix);
	Out.Color = uniformColor;

	Out.PosW = PosW.xyz;
	Out.NormalW = Normal;
	Out.TangentW = mul(Input.Tangent.xyz, (float3x3)worldMatrix);
	Out.BinormalW = cross(Out.TangentW, Out.NormalW) * Input.Tangent.w;
	Out.UV = Input.UV;

	return Out;
}
