#include "pch.h"

#include "Motor.h"
#include "../System/PhysicsSystem.h"
#include "Core/ComponentFactory.h"
#include "Transform.h"
#include "Core/Entity.h"
#include "Core/TimeManager.h"

namespace Hourglass
{
	uint32_t Motor::s_TypeID = ComponentFactory::GetSystemComponentID();

	void Motor::LoadFromXML( tinyxml2::XMLElement* data )
	{		
		m_TurnSpeed = 0.0f;
		bool moveEnabled = true;

		data->QueryFloatAttribute( "max_speed", &m_MaxSpeed );

		m_Speed = m_MaxSpeed;
		data->QueryFloatAttribute( "init_speed", &m_Speed );
		
		data->QueryFloatAttribute( "turn_speed", &m_TurnSpeed );
		data->QueryBoolAttribute( "move_enabled", &moveEnabled );

		bool maxSpeedOverride = false;
		data->QueryBoolAttribute( "max_speed_override", &maxSpeedOverride );

		//m_DesiredForward = Vector3::Forward;
		m_MoveEnabled = moveEnabled;
	}

	void Motor::Update()
	{
		Transform& trans = *GetEntity()->GetTransform();
		
		if (FLT_EQUALS_ZERO(m_DesiredForward.LengthSquared()))
			m_DesiredForward = trans.Forward();

		Vector3 worldPos = trans.GetWorldPosition();
		Vector3 toVector = worldPos + m_DesiredForward;
		toVector.y = worldPos.y;
		trans.RotateTo( trans.BuildLookAtRotation( toVector, worldPos ), m_TurnSpeed );

		if (m_MoveEnabled)
		{
			m_DesiredMove.y = 0.0f;
			Vector3 dv = m_DesiredMove * m_Speed * g_Time.Delta();

			if (m_DynCollider == nullptr)
			{
				m_DynCollider = GetEntity()->GetComponent<DynamicCollider>();
			}

			m_DynCollider->Move( dv );
		}		
	}

	IComponent* Motor::MakeCopyDerived() const
	{
		Motor* cpy = (Motor*)IComponent::Create( SID( Motor ) );

		cpy->m_DynCollider = nullptr;
		cpy->m_DesiredForward = m_DesiredForward;
		cpy->m_DesiredMove = m_DesiredMove;
		cpy->m_MoveEnabled = m_MoveEnabled;
		cpy->m_Speed = m_Speed;
		cpy->m_MaxSpeed = m_MaxSpeed;
		cpy->m_TurnSpeed = m_TurnSpeed;

		return cpy;
	}
}