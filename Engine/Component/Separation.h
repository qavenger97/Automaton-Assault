#pragma once

namespace Hourglass
{
	class Separation : public hg::IComponent
	{
	public:

		enum SeparationMode
		{
			kAgent = 0,
			kObstacle = 1
		};

		virtual int GetTypeID() const { return s_TypeID; }
		static uint32_t s_TypeID;

		void LoadFromXML( tinyxml2::XMLElement* data );

		float GetSeparationStrength() const { return m_SeparationStrength; }
		int GetMode() const { return m_Mode; }
		const Vector3& GetDesiredMove() const { return m_DesiredMove; }
		float GetSafeRadius() const { return m_SafeRadius; }

		void SetDesiredMove( Vector3 dir ) { m_DesiredMove = dir; }
		void SetMode( SeparationMode mode ) { m_Mode = mode; }
		void SetSeparationStrength( float strength ) { m_SeparationStrength = strength; }

		hg::IComponent* MakeCopyDerived() const;

	private:

		float m_SafeRadius;
		float m_SeparationStrength;
		int m_Mode;
		Vector3 m_DesiredMove;
	};
}