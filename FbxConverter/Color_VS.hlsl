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
	float4 PosL		: POSITION;
	float4 Color	: COLOR0;
};

struct OUTPUT_VERTEX
{
	float4 Color	: COLOR;
	float4 PosH		: SV_POSITION;
};

OUTPUT_VERTEX main(INPUT_VERTEX Input)
{
	OUTPUT_VERTEX Out = (OUTPUT_VERTEX)0;

	Out.PosH = mul(Input.PosL, worldMatrix);
	Out.PosH = mul(Out.PosH, viewProjMatrix);
	Out.Color = Input.Color;

	return Out;
}
