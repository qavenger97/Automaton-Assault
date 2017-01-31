AppendStructuredBuffer<uint>			deadList	: register(u0);

[numthreads(256,1,1)]
void main(uint3 dispatchID : SV_DispatchThreadID)
{
	deadList.Append(dispatchID.x);
}