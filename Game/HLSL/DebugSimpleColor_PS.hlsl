
#include "SharedDefines.h"

struct OUTPUT_VERTEX
{
	float4 PosH		 : SV_POSITION;
};

float4 main(OUTPUT_VERTEX Input) : SV_TARGET
{
	return float4(0.5, 0.5, 0.5, 1.0);
}