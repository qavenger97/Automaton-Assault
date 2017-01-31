#include "pch.h"

float Math::RNG::Range(float min_value, float max_value)
{
	if(min_value > max_value)
		std::swap(min_value, max_value);
	return (float)rand()/RAND_MAX *  (max_value - min_value) + min_value;
}

int Math::RNG::Range( int min_value, int max_value )
{
	if (min_value > max_value)
		std::swap( min_value, max_value );
	return min_value + (rand() % (max_value - min_value + 1));
}

XMVECTOR Math::RNG::RandomVectorRange(const XMVECTOR& v1,const XMVECTOR& v2)
{
	return XMVectorSet(
		Range(XMVectorGetX(v1), XMVectorGetX(v2)),
		Range(XMVectorGetY(v1), XMVectorGetY(v2)),
		Range(XMVectorGetZ(v1), XMVectorGetZ(v2)),
		0);
}

XMVECTOR Math::RNG::RandomOnSphere()
{
	XMVECTOR v = XMVectorSet((float)rand(), (float)rand(), (float)rand(), 0);
	return XMVector3Normalize(v);
}

XMVECTOR Math::RNG::RandomInSphere()
{
	return RandomOnSphere() * Range(-1.0f,1.0f);
}

XMVECTOR Math::RNG::RandomInBoundingBox(const XMVECTOR & origin, const XMVECTOR & extent)
{
	return origin + RandomVectorRange(-extent, extent);
}

XMVECTOR Math::RNG::RandomInBoundingBox(const XMVECTOR & origin, const XMVECTOR & extent, const XMVECTOR & orientation)
{
	return origin + XMVector3Rotate(RandomVectorRange(-extent, extent), orientation);
}

void Math::Gravity(XMVECTOR & position, XMVECTOR & velocity, float dt, const XMVECTOR & acceleration)
{
	velocity += G_GRAVITY*dt;
	position += velocity*dt;
}

void Math::NoGravity(XMVECTOR & position, XMVECTOR & velocity, float dt, const XMVECTOR & acceleration)
{
	position += velocity*dt;
}

void Math::Integrate(XMVECTOR & position, XMVECTOR & velocity, float dt, const XMVECTOR & acceleration)
{
	velocity += acceleration*dt;
	position += velocity*dt;
}

int Math::Alignment(int value, int alignment)
{
	return (value + alignment - 1) & ~(alignment - 1);
}

float Math::Lerp( float lhs, float rhs, float t )
{
	return lhs + t * (rhs - lhs);
}

Vector3 Math::Slerp( const Vector3 & a, const Vector3 & b, float f )
{
	float dot = a.Dot( b );

	if (dot < -1) dot = -1;
	if (dot > 1) dot = 1;

	float t = acosf( dot ) * f;
	Vector3 v = b - a * dot;
	v.Normalize();
	return ((a * cosf( t )) + (v * sinf( t )));
}
