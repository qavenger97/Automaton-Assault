#include "GamePch.h"
#include "IKObject.h"
#include "IKComponent.h"
#include "Projectile.h"
uint32_t IKObject::s_TypeID = hg::ComponentFactory::GetGameComponentID();

IKObject::IKObject()
	:
	m_ik(nullptr),
	m_parent (nullptr),
	m_child (nullptr),
	m_length (0)
{
}

hg::Entity* IKObject::Create(XMVECTOR pos, XMVECTOR dir, float length, StrID ikInstanceName, IKObject * parent /*= nullptr */, IKComponent* ikComponent /*= nullptr */)
{
	hg::Entity* ik = hg::Entity::Assemble(ikInstanceName, pos, dir);
	if (ik)
	{
		IKObject* ikComp = ik->GetComponent<IKObject>();
		ikComp->m_parent = parent;
		if(parent)
			parent->m_child = ikComp;
		ikComp->m_child = nullptr;
		ikComp->m_length = length;
		ikComp->m_ik = ikComponent;
	}
	return ik;
}

hg::Entity* IKObject::Create( XMVECTOR pos, XMVECTOR dir, float length, hg::Entity* entity, IKObject * parent /*= nullptr */, IKComponent* ikComponent /*= nullptr */ )
{
	if (entity)
	{		
		IKObject* ikComp = entity->GetComponent<IKObject>();
		ikComp->m_parent = parent;
		if (parent)
			parent->m_child = ikComp;
		ikComp->m_child = nullptr;
		ikComp->m_length = length;
		ikComp->m_ik = ikComponent;
	}
	return entity;
}

hg::Entity* IKObject::CreateFront( XMVECTOR pos, XMVECTOR dir, float length, StrID ikInstanceName, IKObject * child /*= nullptr */, IKComponent* ikComponent /*= nullptr */ )
{
	hg::Entity* ik = hg::Entity::Assemble( ikInstanceName, pos, dir );
	if (ik)
	{
		IKObject* ikComp = ik->GetComponent<IKObject>();
		ikComp->m_parent = nullptr;
		if (child)
			child->m_parent = ikComp;
		ikComp->m_child = child;
		ikComp->m_length = length;
		ikComp->m_ik = ikComponent;
	}
	return ik;
}

void IKObject::PointTo(hg::Transform * target)
{
	hg::Transform* trans = GetEntity()->GetTransform();
	XMVECTOR dir = target->GetWorldPosition() - trans->GetWorldPosition();
	dir = hg::Transform::BuildLookAtRotationFromDirectionVector(dir,trans->Up());
	trans->SetRotation(dir);
}

void IKObject::DragTo(hg::Transform* target, float dragSpeed /*=5.0f*/)
{
	PointTo(target);
	hg::Transform* trans = GetEntity()->GetTransform();
	auto* dc = GetEntity()->GetComponent<hg::DynamicCollider>();
	if (dc)
	{
		//dc->Move(XMVectorLerp(trans->GetWorldPosition(), target->GetWorldPosition() - trans->Forward() * m_length, hg::g_Time.Delta() * 5));
		dc->Move((target->GetWorldPosition() - trans->Forward() * m_length-trans->GetWorldPosition()) * hg::g_Time.Delta() * 5);
	}
	else
		trans->MoveTo(target->GetWorldPosition() - trans->Forward() * m_length, dragSpeed);
	if (m_parent)
	{
		m_parent->DragTo(GetEntity()->GetTransform(), dragSpeed);
	}
}

void IKObject::DragToReversed( hg::Transform* target, float dragSpeed /*=5.0f*/ )
{
	PointTo( target );
	hg::Transform* trans = GetEntity()->GetTransform();
	hg::DynamicCollider* dy = GetEntity()->GetComponent<hg::DynamicCollider>();

	//trans->MoveTo( target->GetWorldPosition() - trans->Forward() * m_length, hg::g_Time.Delta() * dragSpeed );
	trans->SetPosition(target->GetWorldPosition() - trans->Forward() * m_length);
	if (m_child)
	{
		m_child->DragToReversed( GetEntity()->GetTransform(), dragSpeed );
	}
}

void IKObject::Drag(hg::Transform * target)
{
	PointTo(target);
	hg::Transform* trans = GetEntity()->GetTransform();
	
	trans->SetPosition(target->GetWorldPosition() - trans->Forward() * m_length);
	if (m_parent)
	{
		m_parent->Drag(GetEntity()->GetTransform());
	}
}

float IKObject::Length() const
{
	return m_length;
}

hg::IComponent * IKObject::MakeCopyDerived() const
{
	IKObject* copy = (IKObject*)IComponent::Create(SID(IKObject));
	copy->m_parent = nullptr;
	copy->m_child = nullptr;
	copy->m_ik = nullptr;
	copy->m_length = m_length;
	return copy;
}

void IKObject::OnMessage(hg::Message * msg)
{
	switch (msg->GetType())
	{
	case GameMessageType::kDeath:
		DeathMessage msg;
		msg.sender = GetEntity();
		m_ik->SendMsg(&msg);
		hg::Transform* etr = GetEntity()->GetTransform();
		unsigned int count = etr->GetChildrenCount();
		for (unsigned int i = 0; i < count; ++i)
		{
			etr->GetChild( i )->UnLink();
		}
		GetEntity()->Destroy();
		break;
	}
}
