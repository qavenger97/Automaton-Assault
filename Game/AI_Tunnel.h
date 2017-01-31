#pragma once

class AI_Tunnel : public Hourglass::IAction
{
public:

	void LoadFromXML( tinyxml2::XMLElement* data );

	bool IsRunningChild() const { return false; }

	void Start();

	void Init( Hourglass::Entity* entity );

	IBehavior::Result Update( Hourglass::Entity* entity );

	void TargetStrikingPosition( hg::Entity* entity );
	void TargetPlayerPosition( hg::Entity* entity );
	
	void PushBackPlayer( hg::Entity* entity );

	IBehavior* MakeCopy() const;

private:

	static const int submergedOffset = 0;
	static const int submergedInterval = 4;

	float m_DiveTime;
	float m_SurfaceInterval;
	float m_DiveLength;
	float m_ArcPrediction;	
	float m_TargetOffsetRadius;

	Vector3 m_Pivot;
	Vector3 m_StartNl;
	Vector3 m_EndNl;

	float m_PushInterval;
	float m_PushTime;
	float m_PushRadius;
	Vector3 m_PushBegin;
	Vector3 m_PushEnd;

	hg::Entity* m_Player;
	hg::Entity* m_MovingEntity;
	StrID m_MovingEntityName;

	uint32_t m_DiveCount;
	uint32_t m_NumDives;

	float m_StartL;
	float m_EndL;
};