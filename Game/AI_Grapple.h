#pragma once

class IKComponent;

class AI_Grapple : public Hourglass::IAction
{
public:
	void LoadFromXML( tinyxml2::XMLElement* data );
	
	void Init( Hourglass::Entity* entity );

	IBehavior::Result Update( Hourglass::Entity* entity );

	IBehavior* MakeCopy() const;

private:

	hg::Transform* m_Grapple;
	hg::Transform* m_Target;

	static const float kFallHeight;

	IKComponent* m_IK;
	float m_RetractSpeed;
	float m_RetractDuration;
	float m_RetractTimer;
	StrID m_TargetTag;
	Vector3 m_StartPos;
};