#pragma once

#include <DirectXMath.h>
#include <DirectXCollision.h>

#include "../Math/SimpleMath.h"

using namespace DirectX;
using namespace DirectX::SimpleMath;

#define FLT_EQUALS_ZERO(a)		(fabsf(a)<FLT_EPSILON)
#define G_FORWARD				XMVectorSet(0,0,1,0)
#define G_UP					XMVectorSet(0,1,0,0)
#define G_RIGHT					XMVectorSet(1,0,0,0)

#define G_CLAMP(low,high,a)		fminf(fmaxf(a, low),high)
#define G_CLAMP01(a)			G_CLAMP(FLT_EPSILON,1,a)
#define G_DEG2RAD				0.0174532925f
#define G_RAD2DEG				57.295779513f
#define G_GRAVITY				XMVectorSet(0,-9.80665f,0,0)
namespace Math
{
	class RNG
	{
	public:
		static float Range(float min, float max);
		static int Range(int min, int max);
		static XMVECTOR RandomVectorRange(const XMVECTOR& v1, const XMVECTOR& v2);
		static XMVECTOR RandomOnSphere();
		static XMVECTOR RandomInSphere();
		static XMVECTOR RandomInBoundingBox(const XMVECTOR& origin, const XMVECTOR& extent);
		static XMVECTOR RandomInBoundingBox(const XMVECTOR& origin, const XMVECTOR& extent, const XMVECTOR& orientation);
	};


	float Lerp( float a, float b, float f );
	Vector3 Slerp( const Vector3& a, const Vector3& b, float f );
	void Gravity(XMVECTOR& position, XMVECTOR& velocity, float dt, const XMVECTOR & acceleration = XMVectorZero());
	void NoGravity(XMVECTOR& position, XMVECTOR& velocity, float dt, const XMVECTOR & acceleration = G_GRAVITY);
	void Integrate(XMVECTOR& position, XMVECTOR& velocity, float dt, const XMVECTOR& acceleration);
	template <typename T> int GetSign(T val) {
		return (T(0) < val) - (val < T(0));
	}
	int Alignment(int value, int alignment);
}