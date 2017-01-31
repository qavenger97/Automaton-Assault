struct INPUT_VERTEX
{
	float3 PosL		: POSITION;
	float4 Color0	: COLOR0;
	float4 Color1	: COLOR1;
	float4 UV		: TEXCOORD0;
};

float4 main(INPUT_VERTEX Input) : SV_POSITION
{
	return float4(0, 0, 0, 0);
}
