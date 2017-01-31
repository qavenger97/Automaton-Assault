struct OUTPUT_VERTEX
{
	float4 Color : COLOR;
};

float4 main(OUTPUT_VERTEX Input) : SV_TARGET
{
	return Input.Color;
}
