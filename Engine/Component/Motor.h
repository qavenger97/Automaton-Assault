#pragma once

#include "IComponent.h"
#include "Core/ComponentRegister.h"
#include "Component\DynamicCollider.h"

namespace Hourglass
{
	class DynamicCollider;

	class Motor : public IComponent
	{
	public:
		
		virtual void LoadFromXML( tinyxml2::XMLElement* data );
		virtual int GetTypeID() const { return s_TypeID; }

		virtual void Update();

		/**
		* Make a copy of this component
		*/
		virtual IComponent* MakeCopyDerived() const;

		void SetMoveEnabled( bool enabled ) { m_MoveEnabled = enabled; }
		void SetSpeed( float speed ) { m_Speed = speed; }
		void SetTurnSpeed( float turnSpeed ) { m_TurnSpeed = turnSpeed; }
		void SetDesiredMove( Vector3 desiredMove ) { m_DesiredMove = desiredMove; }
		void SetDesiredForward( Vector3 desiredForward ) { m_DesiredForward = desiredForward; }

		const Vector3& GetDesiredMove() { return m_DesiredMove; }
		float GetSpeed() const { return m_Speed; }
		float GetMaxSpeed() const { return m_MaxSpeed; }

		static uint32_t s_TypeID;

	private:

		float m_Speed;
		float m_MaxSpeed;
		float m_TurnSpeed;
		Vector3 m_DesiredMove;
		Vector3 m_DesiredForward;

		DynamicCollider* m_DynCollider;

		uint32_t m_MoveEnabled : 1;
	};
}