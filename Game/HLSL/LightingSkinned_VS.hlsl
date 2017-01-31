
#include "ConstBufferVS.h"

struct INPUT_VERTEX
{
	float3 PosL		  : POSITION;
	float2 UV		  : TEXCOORD0;
	float3 Normal	  : NORMAL;
	float4 Tangent	  : TANGENT;

	int4   Influences : INFLUENCES;
	float4 Weights	  : WEIGHTS;
};

struct OUTPUT_VERTEX
{
	float4 Color	  : COLOR;
	float4 PosH		  : SV_POSITION;
	float2 UV		  : TEXCOORD0;
	float3 PosW		  : TEXCOORD1;		// World space position
	float3 NormalW	  : TEXCOORD2;		// World space normal
	float3 TangentW	  : TEXCOORD3;
	float3 BinormalW  : TEXCOORD4;
};

OUTPUT_VERTEX main(INPUT_VERTEX Input)
{
	OUTPUT_VERTEX Out = (OUTPUT_VERTEX)0;

	float4x4 skinned = jointPoses[Input.Influences.x] * Input.Weights.x
					 + jointPoses[Input.Influences.y] * Input.Weights.y
					 + jointPoses[Input.Influences.z] * Input.Weights.z
					 + jointPoses[Input.Influences.w] * Input.Weights.w;

	float4 PosW = mul(float4(Input.PosL, 1.0f), skinned);
	PosW = mul(PosW, worldMatrix);
	
	Out.PosH = mul(PosW, viewProjMatrix);

	float3 Normal = mul(Input.Normal, (float3x3)skinned); 
	Normal = mul(Normal, (float3x3)worldMatrix);
	Out.Color = uniformColor;
	
	Out.PosW = PosW.xyz;
	Out.NormalW = Normal;
	Out.TangentW = mul(Input.Tangent.xyz, (float3x3)skinned);
	Out.TangentW = mul(Out.TangentW, (float3x3)worldMatrix) ;
	Out.BinormalW = cross(Out.TangentW, Out.NormalW)* -Input.Tangent.w;
	Out.UV = Input.UV;

	return Out;
}
