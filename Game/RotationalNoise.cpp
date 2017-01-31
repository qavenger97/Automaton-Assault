#include "GamePch.h"

#include "RotationalNoise.h"

uint32_t RotationalNoise::s_TypeID = hg::ComponentFactory::GetGameComponentID();;

void RotationalNoise::LoadFromXML( tinyxml2::XMLElement* data )
{
	m_MaxAngle = 0.0872665f; // 5 degrees
	data->QueryFloatAttribute( "max_angle", &m_MaxAngle );

	m_NoiseLow = 0.2f;
	m_NoiseHigh = 3.0f;

	data->QueryFloatAttribute( "noise_low", &m_NoiseLow );
	data->QueryFloatAttribute( "noise_high", &m_NoiseHigh );
}

void RotationalNoise::Init()
{
	m_Timer = 0.0f;
	m_Yaw = 0.0f;
	m_Pitch = 0.0f;
	m_Trans = GetEntity()->GetTransform();
}

void RotationalNoise::Update()
{
	m_Timer -= hg::g_Time.Delta();
	if (m_Timer <= 0.0f)
	{
		m_Timer = Math::RNG::Range( m_NoiseLow, m_NoiseHigh );
		m_Yaw = Math::RNG::Range( -m_MaxAngle, m_MaxAngle );
		m_Pitch = Math::RNG::Range( -m_MaxAngle, m_MaxAngle );
		m_IntervalInverse = 1.0f / m_Timer;
	}

	float rx = m_Timer * m_IntervalInverse * XM_PI;
	float ry = sinf( rx );

	//Quaternion rot = m_Trans->GetLocalRotation();
	//float roll = atan2f( 2.0f * rot.y * rot.w - 2.0f * rot.x * rot.z, 1.0f - 2.0f * rot.y * rot.y - 2.0f * rot.z * rot.z );
	Quaternion rot = Quaternion::CreateFromYawPitchRoll( ry * m_Pitch, ry * m_Yaw, 0.0f );
	rot *= m_Trans->GetParent()->GetWorldRotation();
	m_Trans->SetRotation( rot );
}

hg::IComponent* RotationalNoise::MakeCopyDerived() const
{
	RotationalNoise* copy = (RotationalNoise*)IComponent::Create( SID(RotationalNoise) );
	copy->m_NoiseLow = m_NoiseLow;
	copy->m_NoiseHigh = m_NoiseHigh;
	copy->m_MaxAngle = m_MaxAngle;

	return copy;
}
