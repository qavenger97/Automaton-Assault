/*!
 * \file SpinMotor.cpp
 *
 * \author Shiyang Ao
 * \date November 2016
 *
 * 
 */
#include "GamePch.h"

#include "SpinMotor.h"

IMPLEMENT_GAME_COMPONENT_TYPEID(SpinMotor)

void SpinMotor::LoadFromXML( tinyxml2::XMLElement* data )
{
	m_SpinSpeed = 3.0f;
	data->QueryFloatAttribute( "speed", &m_SpinSpeed);
	m_Shifting = false;
	data->QueryBoolAttribute( "start_active", &m_Shifting );

	m_SpinType = kYaw;
	const char* spinAxis = data->Attribute( "spin_type");

	if (spinAxis)
	{
		if (strcmp( spinAxis, "pitch" ) == 0)
		{
			m_SpinType = kPitch;
		}
		else if (strcmp( spinAxis, "roll" ) == 0)
		{
			m_SpinType = kRoll;
		}
	}

	m_FixedEuler = Vector3::Zero;
	data->QueryFloatAttribute( "fixed_pitch", &m_FixedEuler.x );
	data->QueryFloatAttribute( "fixed_yaw", &m_FixedEuler.y );
	data->QueryFloatAttribute( "fixed_roll", &m_FixedEuler.z );
}

void SpinMotor::Init()
{
	m_CurrentRadians = 0;
}

void SpinMotor::Update()
{
	if (m_Shifting)
	{
		hg::Entity* ent = GetEntity();

		float t = hg::g_Time.Delta() * -1.0f * XMConvertToRadians(m_SpinSpeed);
		m_CurrentRadians += t;

		if (m_CurrentRadians >= XM_2PI)
		{
			m_CurrentRadians -= XM_2PI;
		}

		if (m_SpinType == kYaw)
		{
			Quaternion rot = Quaternion::CreateFromYawPitchRoll( m_CurrentRadians, m_FixedEuler.y, m_FixedEuler.z);
			rot *= ent->GetParent()->GetTransform()->GetWorldRotation();
			ent->GetTransform()->SetRotation( rot );
		}
		else if (m_SpinType == kPitch)
		{
			ent->GetTransform()->SetRotation( m_FixedEuler.y, m_CurrentRadians, m_FixedEuler.z );
		}
		else
		{
			Quaternion rot = Quaternion::CreateFromYawPitchRoll( m_FixedEuler.x, m_FixedEuler.y, m_CurrentRadians );
			rot *= ent->GetParent()->GetTransform()->GetWorldRotation();
			ent->GetTransform()->SetRotation( rot );
		}
	}
}

void SpinMotor::StartSpin()
{
	m_Shifting = true;
}

void SpinMotor::StopSpin()
{
	m_Shifting = false;
}

hg::IComponent* SpinMotor::MakeCopyDerived() const
{
	SpinMotor* copy = (SpinMotor*)IComponent::Create(SID(SpinMotor));
	copy->m_Shifting = false;
	copy->m_CurrentRadians = m_CurrentRadians;
	copy->m_SpinType = m_SpinType;

	return copy;
}
