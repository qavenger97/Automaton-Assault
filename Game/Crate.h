#pragma once

class Crate : public hg::IComponent
{
	DECLARE_COMPONENT_TYPEID
public:
	void LoadFromXML(tinyxml2::XMLElement* data);

	void Update();

	hg::IComponent* MakeCopyDerived() const;

	void OnMessage(hg::Message* msg);

	void SpawnPresetEntity();

private:
	StrID	m_SpawnEntity;

	float	m_VelocityY;
};
