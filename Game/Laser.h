#pragma once

class Laser : public hg::IComponent
{
	DECLARE_COMPONENT_TYPEID
public:
	void LoadFromXML( tinyxml2::XMLElement* data );
	hg::IComponent* MakeCopyDerived() const;
	void Update();
private:

	float m_Radius;
	uint32_t m_CollMask;
};