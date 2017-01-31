#pragma pack_matrix(row_major)

cbuffer cbScene : register(b0)
{
	float4x4	viewProjMatrix;
}

cbuffer cbPerObject : register(b1)
{
	float4x4	worldMatrix;
}

struct INPUT_VERTEX
{
	float3 PosL		: POSITION;
	float2 UV		: TEXCOORD0;
	float3 Normal	: NORMAL;
	float4 Tangent	: TANGENT;
};

struct OUTPUT_VERTEX
{
	float4 Color	: COLOR;
	float4 PosH		: SV_POSITION;
};

OUTPUT_VERTEX main(INPUT_VERTEX Input)
{
	OUTPUT_VERTEX Out = (OUTPUT_VERTEX)0;

	Out.PosH = mul(float4(Input.PosL, 1.0f), worldMatrix);
	Out.PosH = mul(Out.PosH, viewProjMatrix);

	float3 NormalW = mul(Input.Normal, (float3x3)worldMatrix);
	Out.Color = saturate(dot(NormalW, float3(0, 1, 0)) * 0.5f + 0.5f);
	Out.Color.a = 1.0f;

	return Out;
}
