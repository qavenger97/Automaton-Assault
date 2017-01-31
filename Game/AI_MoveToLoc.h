#pragma once

class AI_MoveToLoc : public Hourglass::IAction
{
public:
	void LoadFromXML( tinyxml2::XMLElement* data );

	void Init( Hourglass::Entity* entity );

	IBehavior::Result Update( Hourglass::Entity* entity );

	IBehavior* MakeCopy() const;

private:

	Vector3 m_TargetLocation;
	float m_CompletionToleranceSq;
	hg::Transform* m_Trans;
	hg::Motor* m_Motor;
};