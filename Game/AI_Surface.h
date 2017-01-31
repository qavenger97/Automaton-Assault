#pragma once

class AI_Surface : public Hourglass::IAction
{
public:
	void LoadFromXML( tinyxml2::XMLElement* data );

	void Init( Hourglass::Entity* entity );

	IBehavior::Result Update( Hourglass::Entity* entity );

	IBehavior* MakeCopy() const;

private:

	StrID m_SurfaceEntityName;
	hg::Entity* m_SurfaceEntity;

	float m_StartSpeed;
	float m_EndSpeed;
	float m_Rotation;
	float m_StartRotation;
	float m_EndRotation;
};