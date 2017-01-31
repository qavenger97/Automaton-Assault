#pragma once

class EntityGroup : public hg::IComponent
{
public:
	virtual int GetTypeID() const { return s_TypeID; }
	static uint32_t s_TypeID;

	void Start();
	void Update();

	void AddEntity( hg::Entity* entity ) { m_Entities.insert(entity); }
	void RemoveEntity( hg::Entity* entity ) { m_Entities.erase( entity ); }

	bool IsAlone() { return m_Entities.empty(); }
	bool IsOneOnOne() { return m_Entities.size() == 1; }

	hg::IComponent* MakeCopyDerived() const;

private:

	std::set<hg::Entity*> m_Entities;
	bool	m_bAloneState;
};