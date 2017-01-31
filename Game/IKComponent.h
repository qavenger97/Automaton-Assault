#pragma once
#include "IKObject.h"

struct IKInstanceInfo
{
	StrID name;
	float length;
};

class IKComponent : public hg::IComponent
{
public:
	virtual int GetTypeID() const { return s_TypeID; }
	hg::IComponent* MakeCopyDerived() const;
public:
	static uint32_t s_TypeID;
public:
	IKComponent();
	void LoadFromXML(tinyxml2::XMLElement * data);
	void Init()override;
	void Start()override;
	void Add(float length);
	void Add( float length, StrID instanceName );
	void Add( float length, hg::Entity* );
	void AddFront(float length);
	void LinkEntity(hg::Entity* obj);
	const FXMVECTOR IKComponent::GetFrontPosition() const;
	const FXMVECTOR IKComponent::GetBackPosition() const;
	float GetLength() const { return m_length; }
	bool IsEmpty() const;
	void SetFrozen( bool frozen );
	void Remove(IKObject* data);
	void RemoveBack();
	void RemoveFront();
	void SetReversed(bool reversed);
	void Update()override;
	void SetTarget(hg::Entity* target);
	void SetSource(hg::Entity* source);
	void SendMsg(hg::Message * msg);
	void Shutdown()override;
private:
	hg::Entity* m_target;
	hg::Entity* m_source;
	std::list<IKObject*> m_ikObjects;
	uint16_t m_numObjs;
	float m_length;
	void(*UpdateFunc)();
	int m_iteration;
	float m_rotation;
	float m_dragSpeed;
	uint32_t m_reversed : 1;
	uint32_t m_frozen : 1;
	uint32_t m_boss : 1;
	
	StrID m_bossName;

	Vector3 m_InitialRotation;

	StrID m_instanceName;
	std::vector<IKInstanceInfo> m_instanceInfos;
private:
	void ChainRotation();
	void Reach();
};

