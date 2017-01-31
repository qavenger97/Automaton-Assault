#pragma once

class SpinMotor;

class RotatingRoom : public hg::IComponent
{
public:
	virtual int GetTypeID() const { return s_TypeID; }
	static uint32_t s_TypeID;

	void LoadFromXML( tinyxml2::XMLElement* data );

	void Init();

	void Start();

	void Update();

	bool IsFinished() { return m_Finished; }

	void SpawnNextGrappler();

	void OnEvent(const StrID& eventName);

	hg::IComponent* MakeCopyDerived() const;

private:

	
	SpinMotor* m_SpinMotor;
	enum{ kNumGrapplers = 4};
	float m_GrapplerStopAngle[kNumGrapplers];
	hg::Entity* m_Grapplers[kNumGrapplers];
	Vector3 m_GrapplerSpawnPoints[kNumGrapplers];
	Vector3 m_GrapplerPatrolPos[kNumGrapplers];
	unsigned int m_CurrentGrapplerId;
	float m_Timer;
	float m_Delay;
	uint32_t m_Finished : 1;
};