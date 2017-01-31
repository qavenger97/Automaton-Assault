#pragma once
class IKComponent;
class IKObject : public hg::IComponent
{
public:
	IKObject();

	static hg::Entity* Create(XMVECTOR pos, XMVECTOR dir, float length, StrID ikInstanceName, IKObject* parent = nullptr, IKComponent* ikComponent = nullptr);
	static hg::Entity* Create( XMVECTOR pos, XMVECTOR dir, float length, hg::Entity* entity, IKObject* parent = nullptr, IKComponent* ikComponent = nullptr );
	static hg::Entity* CreateFront( XMVECTOR pos, XMVECTOR dir, float length, StrID ikInstanceName, IKObject* parent = nullptr, IKComponent* ikComponent = nullptr );
	void PointTo(hg::Transform* target);
	void DragTo(hg::Transform* target, float dragSpeed = 5.0f);
	void DragToReversed( hg::Transform* target, float dragSpeed = 5.0f );
	void Drag(hg::Transform* target);

	virtual int GetTypeID() const { return s_TypeID; }
	float Length()const;
	hg::IComponent* MakeCopyDerived() const;

	void OnMessage(hg::Message* msg)override;
public:
	static uint32_t s_TypeID;
	friend IKComponent;
private:
	IKComponent* m_ik;
	IKObject* m_parent;
	IKObject* m_child;
	float m_length;
};

