struct INPUT_VERTEX
{
	float3 PosL		  : POSITION;
	float2 UV		  : TEXCOORD0;
	float3 Normal	  : NORMAL;
	float4 Tangent	  : TANGENT;
	int4   Influences : INFLUENCES;
	float4 Weights	  : WEIGHTS;
};

float4 main(INPUT_VERTEX Input) : SV_POSITION
{
	return float4(0, 0, 0, 0);
}
