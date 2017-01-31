#pragma once

class AI_Lunge : public Hourglass::IAction
{
public:

	void LoadFromXML( tinyxml2::XMLElement* data );

	void Init( Hourglass::Entity* entity );

	void Shutdown( Hourglass::Entity* entity );

	IBehavior::Result Update( Hourglass::Entity* entity );

	IBehavior* MakeCopy() const;

private:

	void Recover();
	bool IsFullyRecovered();
	bool IsAtDestination();
	void PosAndColorShadowCopies(const Vector3& movementDir);

	StrID		m_Anim;
	float		m_Dist;
	float		m_Speed;
	float		m_Traveled;
	float		m_ShadowLifetime;
	float		m_ShadowRadius;
	float		m_RecoverTimer;
	float		m_RecoverLength;
	int			m_ShadowSegments;
	Vector3		m_LungeStart;
	Color		m_ShieldColor;
	Color		m_ShadowColor;
	uint32_t	m_NumShadows;

	uint32_t	m_Inverted : 1;
};