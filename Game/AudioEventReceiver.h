#pragma once

class AudioEventReceiver : public hg::IComponent
{
	DECLARE_COMPONENT_TYPEID
public:
	void LoadFromXML(tinyxml2::XMLElement* data);

	hg::IComponent* MakeCopyDerived() const;

	void OnEvent(const StrID& eventName);
private:
	StrID	m_EventName;
};
