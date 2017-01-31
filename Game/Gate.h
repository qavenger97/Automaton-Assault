#pragma once

enum GateStatus
{
	kGate_Shut,
	kGate_Opening,
	kGate_Open,
	kGate_Closing,
};

class Gate : public Hourglass::IComponent
{
	DECLARE_COMPONENT_TYPEID
public:

	void Init();

	void LoadFromXML(tinyxml2::XMLElement* data);

	void Update();

	void OnEvent(const StrID& eventName);

	hg::IComponent* MakeCopyDerived() const;

	void Open();

private:
	// XML attributes
	Vector3		m_TargetOffset;
	float		m_OpenTime;				// Total time before gate fully open
	int			m_Status;

	// Gate members
	float		m_Timer;				// Timer for gate running
};
