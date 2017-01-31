#pragma once
class PlayerComp;
class Pickup : public Hourglass::IComponent
{
public:
	enum Type
	{
		AMMO,
		HEALTH,
		RICOCHET,
		SHIELD,
		BOSSTRIGGER,
		NUM_TYPE
	};
	void Start() override;
	void Update() override;

	virtual int GetTypeID() const override { return s_TypeID; }

	void LoadFromXML(tinyxml2::XMLElement* data);

	virtual hg::IComponent* MakeCopyDerived() const;

	static uint32_t s_TypeID;
private:
	PlayerComp* player;
	float m_value;
	Type m_type;
};

