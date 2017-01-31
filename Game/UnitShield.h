#pragma once

class UnitShield : public hg::IComponent
{
public:
	virtual int GetTypeID() const { return s_TypeID; }
	static uint32_t s_TypeID;

	void LoadFromXML( tinyxml2::XMLElement* data );

	const Vector3& GetOffset() const { return m_Offset; }
	
	float GetRadius() const { return m_Radius; }

	int GetMaxSengments() const { return m_MaxSegments; }

	void Start();

	void Update();

	hg::IComponent* MakeCopyDerived() const;

private:

	Color m_Color;
	Vector3 m_Offset;
	float m_Radius;
	int m_MaxSegments;
};