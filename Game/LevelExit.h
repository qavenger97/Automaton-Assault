#pragma once

class LevelExit : public hg::IComponent
{
public:
	virtual int GetTypeID() const { return s_TypeID; }

	void LoadFromXML(tinyxml2::XMLElement* data);

	//virtual void Init();
	virtual void Update();

	hg::IComponent* MakeCopyDerived() const;

	static uint32_t s_TypeID;
private:
	Vector3	m_Size;			// Size of trigger area in all three direction
};

