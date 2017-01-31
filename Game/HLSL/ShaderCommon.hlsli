#ifndef _SHADERCOMMON_H
#define _SHADERCOMMON_H

#define FLT_MAX		3.402823466e+38F
#define DIVPI		0.318309886f
#define PI			3.141592654f

const static float kDielectricConst = 0.04;
const static float kPI = 3.1415926535897932;
const static float kEpsilon = 1e-3;

float3 ComputeHalfVector(float3 v1, float3 v2)
{
	return normalize(v1 + v2);
}

float Random01(uint2 id, uint seed)
{
	uint rs = (0x27d4eb2d * id.x) ^ (seed ^ 61) ^ (seed >> 16);
	rs *= 9;
	rs = rs ^ (rs >> 4);
	rs *= (0x27d4eb2d ^ id.x);
	rs = rs ^ (rs >> 15);
	return ((float)rs / 0xffffffff);
}

float Random(float min, float max, uint2 id, uint seed)
{
	return (max - min) * Random01(id, seed) + min;
}

float3 RandomOnUnitSphere(uint2 id, uint seed)
{
	float x = Random01(id, seed + id.x + 1);
	float y = Random01(id, seed + id.x + 0x27d4eb2d);
	float z = Random01(id, seed - id.x + 0x27d4eb2d);
	return normalize((float3(x, y, z)-0.5)*2);
}

float3 RandomVector3(float3 lhs, float3 rhs, uint2 id, uint seed)
{
	float x = Random(lhs.x, rhs.x, id, seed + id.x + 1);
	float y = Random(lhs.y, rhs.y, id, seed + id.x + 0x27d4eb2d);
	float z = Random(lhs.z, rhs.z, id, seed - id.x + 0x27d4eb2d);
	return float3(x, y, z);
}

float4 RandomVector4(float4 lhs, float4 rhs, uint2 id, uint seed)
{
	float x = Random(lhs.x, rhs.x, id, seed + id.x + 1);
	float y = Random(lhs.y, rhs.y, id, seed + id.x + 0x27d4eb2d);
	float z = Random(lhs.z, rhs.z, id, seed - id.x - 0x27d4eb2d);
	float w = Random(lhs.w, rhs.w, id, seed + 0x27d4eb2d);
	return float4(x, y, z, w);
}

float4 MulQuaternion(float4 q1, float4 q2)
{
	float4 rs = float4(0, 0, 0, 0);
	rs.w = q1.w * q2.w - dot(q1.xyz, q2.xyz);
	rs.xyz = q1.w * q2.xyz + q2.w * q1.xyz + cross(q1.xyz, q2.xyz);
	return rs;
}

float3 MulQuaternionVector(float4 q, float3 v)
{
	float3 t = 2.0 * cross(q.xyz, v);
	return v + q.w * t + cross(q.xyz, t);
}

float3 TransformPosition(float3 v, float4 q, float3 t)
{
	return MulQuaternionVector(q, v) + t;
}

#endif