#pragma once
#include "HealthModule.h"
#include "../Engine/System/EntityManager.h"
class BossAgent : public hg::IComponent
{
public:

	void Start()override;
	static uint32_t s_TypeID;

	virtual int GetTypeID() const override { return s_TypeID; }
	void LoadFromXML(tinyxml2::XMLElement* data) {}

	virtual hg::IComponent* MakeCopyDerived() const { return hg::IComponent::Create(SID(BossComp)); }
	void Update()override;
private:
	Health* m_health;
	hg::Entity* m_Bar;
};