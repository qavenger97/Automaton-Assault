#pragma once

class RotatingRoom;

class ShiftingPlatform : public hg::IComponent
{
public:
	virtual int GetTypeID() const { return s_TypeID; }
	static uint32_t s_TypeID;

	void LoadFromXML( tinyxml2::XMLElement* data );

	void Start();

	void Update();

	void OnEvent(const StrID& eventName);

	void Extend();

	void Retract();

	bool IsExtending() { return m_ShiftActivated != 0 && m_Extended == 0; }

	bool IsRetracting() { return m_ShiftActivated != 0 && m_Extended != 0; }

	hg::IComponent* MakeCopyDerived() const;

private:

	uint32_t m_Extended : 1;
	uint32_t m_ShiftActivated : 1;
	uint32_t m_WaitForExtend : 1;
	Vector3 m_ExtendVector;	
	Vector3 m_ShiftStart;
	Vector3 m_ShiftEnd;
	RotatingRoom* m_RotRoom;

	float m_ShiftLength;
	float m_ShiftLengthInv;
	float m_Timer;
};