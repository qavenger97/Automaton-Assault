#pragma once
class BossComp : public hg::IComponent
{
public:
	void Start();
	virtual int GetTypeID() const override { return s_TypeID; }
	void LoadFromXML(tinyxml2::XMLElement* data) {}

	virtual hg::IComponent* MakeCopyDerived() const { return hg::IComponent::Create(SID(BossComp)); }

	static uint32_t s_TypeID;
	void OnMessage(hg::Message* msg)override;
private:
	static hg::Entity* s_agent;
};

