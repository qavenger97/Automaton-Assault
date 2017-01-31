#pragma once
class PowerUpShield : public hg::IComponent
{
public:
	void Start()override;
	void Update()override;
	virtual int GetTypeID() const { return s_TypeID; }

	void LoadFromXML(tinyxml2::XMLElement* data);

	virtual hg::IComponent* MakeCopyDerived() const;

	static uint32_t s_TypeID;
private:
	float m_timer;
	float m_duration;
};

