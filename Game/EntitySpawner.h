#pragma once

class EntitySpawner : public hg::IComponent
{
public:
	virtual int GetTypeID() const { return s_TypeID; }
	static uint32_t s_TypeID;

	void LoadFromXML(tinyxml2::XMLElement* data);

	void Init();

	void Spawn();

	void Update();

	hg::IComponent* MakeCopyDerived() const;

	void OnEvent(const StrID& eventName);
private:
	StrID	m_AssembledName;		// Name for assembled entity to be spawned

	bool	m_bUseTarget;
	Vector3	m_TargetPosition;

	bool	m_SpawnOnStart;
	bool	m_TimerStarted;
	bool	m_Spawned;
	float	m_Delay;
	float	m_Timer;
};
