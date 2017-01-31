#pragma once

class Terminal : public Hourglass::IComponent
{
	DECLARE_COMPONENT_TYPEID
public:
	void Init();

	void LoadFromXML(tinyxml2::XMLElement* data);

	void Update();

	void OnTriggerEnter(hg::Entity* other);
	void OnTriggerExit(hg::Entity* other);

	hg::IComponent* MakeCopyDerived() const;

private:
	bool	m_bPlayerInRange;
	std::vector<hg::TriggerEvent>		m_EventTargetIds;
};