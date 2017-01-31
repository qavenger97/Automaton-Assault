#include "GamePch.h"
#include "IKComponent.h"
#include "Projectile.h"
uint32_t IKComponent::s_TypeID = hg::ComponentFactory::GetGameComponentID();

IKComponent::IKComponent()
	:
	m_target(nullptr),
	UpdateFunc(nullptr),
	m_iteration(0)
{
}

void IKComponent::LoadFromXML(tinyxml2::XMLElement* data)
{
	int num = 0;
	m_rotation = 0.0;
	m_dragSpeed = 5.0f;
	data->QueryIntAttribute("Count", &num);
	m_numObjs = num;
	data->QueryFloatAttribute("Length", &m_length);
	data->QueryFloatAttribute("rotation", &m_rotation);
	data->QueryFloatAttribute("drag_speed", &m_dragSpeed);
	
	const char* instanceName = data->Attribute("instance_name");

	m_instanceName = instanceName ? WSID(instanceName) : SID(IKEntity);	

	bool isReversed = false;
	data->QueryBoolAttribute("reversed", &isReversed);
	m_reversed = isReversed;

	bool isFrozen = false;
	data->QueryBoolAttribute("frozen", &isFrozen);
	SetFrozen(isFrozen);
	data->QueryIntAttribute("Iteration", &m_iteration);

	m_boss = false;
	const char* bossName = data->Attribute( "boss_name" );	

	if (bossName)
	{
		m_boss = true;
		m_bossName = WSID(bossName);
	}
	else
	{
		tinyxml2::XMLElement* instanceXML = data->FirstChildElement( "IKInstance" );

		m_instanceInfos.clear();

		while (instanceXML)
		{
			IKInstanceInfo newInfo;
			const char* name = instanceXML->Attribute( "name" );
			if (name)
				newInfo.name = WSID( name );
			else
				newInfo.name = WSID( "" );

			newInfo.length = 1.5f;
			instanceXML->QueryFloatAttribute( "length", &newInfo.length );
			m_instanceInfos.push_back( newInfo );
			instanceXML = instanceXML->NextSiblingElement();
		}
	}	
}

void IKComponent::Init()
{
}

void IKComponent::Start()
{
	Shutdown();
	m_ikObjects.clear();
	SetTarget(GetEntity());

	if (m_instanceInfos.empty())
	{
		if (m_boss)
		{
			hg::Entity* boss = hg::Entity::FindByName( m_bossName );
			if (boss)
			{
				hg::Transform* bosstf = boss->GetTransform();
				unsigned int childCount = bosstf->GetChildrenCount();
				float* lengths = new float[childCount];
				float totalLength = 0.0f;
				for (unsigned int i = 0; i < childCount; ++i)
				{
					hg::Transform* child = bosstf->GetChild( i );
					lengths[i] = -totalLength - Vector3( child->GetLocalPosition() ).z;
					totalLength += lengths[i];					
				}

				for (int i = childCount-1; i >= 0; --i)
				{
					hg::Transform* child = bosstf->GetChild( i );
					Add( lengths[i], child->GetEntity() );
				}

				delete[] lengths;
			}
		}
		else
		{
			for (uint16_t i = 0; i < m_numObjs; i++)
				Add( m_length );
		}
	}
	else
	{
		for (IKInstanceInfo& instanceName : m_instanceInfos)
			Add( instanceName.length, instanceName.name );
	}
}

void IKComponent::Add(float length)
{
	hg::Transform* trans = GetEntity()->GetTransform();
	//m_ikObjects.push_back(IKObject::Create(trans->GetWorldPosition(), trans->GetWorldRotation(), length, m_ikObjects.size() ? m_ikObjects.back() : nullptr, this)->GetComponent<IKObject>());
	m_ikObjects.push_back(IKObject::Create(trans->GetWorldPosition(), trans->GetWorldRotation(), length, m_instanceName, m_ikObjects.size() ? m_ikObjects.back() : nullptr, this)->GetComponent<IKObject>());
}

void IKComponent::Add( float length, StrID instanceName )
{
	hg::Transform* trans = GetEntity()->GetTransform();
	m_ikObjects.push_back( IKObject::Create( trans->GetWorldPosition(), trans->GetWorldRotation(), length, instanceName, m_ikObjects.size() ? m_ikObjects.back() : nullptr, this )->GetComponent<IKObject>() );
}

void IKComponent::Add( float length, hg::Entity* entity )
{
	hg::Transform* trans = GetEntity()->GetTransform();
	m_ikObjects.push_back( IKObject::Create( trans->GetWorldPosition(), trans->GetWorldRotation(), length, entity, m_ikObjects.size() ? m_ikObjects.back() : nullptr, this )->GetComponent<IKObject>() );
}

void IKComponent::AddFront( float length )
{
	hg::Transform* trans = GetEntity()->GetTransform();
	m_ikObjects.push_front( IKObject::CreateFront( trans->GetWorldPosition(), trans->GetWorldRotation(), length, m_instanceName, m_ikObjects.size() ? m_ikObjects.front() : nullptr, this )->GetComponent<IKObject>() );
}

void IKComponent::LinkEntity( hg::Entity* entity )
{
	//entity->GetTransform()->SetPosition( m_ikObjects.front()->GetEntity()->GetTransform()->GetWorldPosition() );
	entity->GetTransform()->LinkTo( m_ikObjects.back()->GetEntity()->GetTransform() );
}

void IKComponent::SetFrozen( bool frozen )
{
	if (frozen)
	{
		hg::Transform* tf = GetEntity()->GetTransform();
		for (auto* o : m_ikObjects)
		{
			o->GetEntity()->GetTransform()->LinkTo(tf);
		}
	}
	else if (frozen != m_frozen)
	{
		hg::Transform* tf = GetEntity()->GetTransform();
		for (auto* o : m_ikObjects)
		{
			o->GetEntity()->GetTransform()->UnLink();
		}
	}

	m_frozen = frozen;
}

void IKComponent::Remove(IKObject* data)
{
	auto itr = m_ikObjects.begin();
	if (data->m_parent)
	{
		auto itr_prev = itr;
		while (*itr != data)
		{
			itr_prev = itr;
			++itr;
		}
		auto itr_next = itr;
		++itr_next;
		if(itr_next != m_ikObjects.end())
			(*itr_next)->m_parent = *itr_prev;
	}
	else
	{
		auto itr_next = itr;
		if (++itr_next != m_ikObjects.end())
		{
			(*itr_next)->m_parent = nullptr;
		}
	}
	m_ikObjects.erase(itr);
}

void IKComponent::RemoveBack()
{
	m_ikObjects.pop_back();
}

void IKComponent::RemoveFront()
{
	auto itr = m_ikObjects.begin();
	if (++itr != m_ikObjects.end())
	{
		(*itr)->m_parent = nullptr;
	}
	m_ikObjects.front()->GetEntity()->Destroy();
	m_ikObjects.pop_front();
}

const FXMVECTOR IKComponent::GetFrontPosition() const
{
	return m_ikObjects.front()->GetEntity()->GetTransform()->GetWorldPosition();
}

const FXMVECTOR IKComponent::GetBackPosition() const
{
	return m_ikObjects.back()->GetEntity()->GetTransform()->GetWorldPosition();
}

bool IKComponent::IsEmpty() const
{
	return m_ikObjects.empty();
}

void IKComponent::SetReversed( bool reversed )
{
	m_reversed = reversed;
}

void IKComponent::Update()
{
	if (hg::g_Time.GetTimeScale() == 0)return;
	//if (m_target)
	if (m_target && !m_frozen)
		if (m_ikObjects.size())
		{
				//m_ikObjects.back()->DragTo(m_target->GetTransform());
			if(m_reversed)
				m_ikObjects.front()->DragToReversed(m_source->GetTransform(), m_dragSpeed);
			else if (m_iteration)
			{
				for (int i = 0; i < m_iteration; i++)
				{
					Reach();
				}
			}
			else
			{
				m_ikObjects.back()->DragTo(m_target->GetTransform(), m_dragSpeed);
			}			
			if (UpdateFunc)
				UpdateFunc();
			static float dt = 0;

			if (m_rotation)
			{
				float rotationModifier = 0.01f;
				float rootRotationModifier = 0.1f;
				float r = m_rotation * dt;
				for (auto* o : m_ikObjects)
				{
					o->GetEntity()->GetTransform()->Rotate(0, 0, r);
					r += m_rotation * hg::g_Time.UnscaledDelta() * rotationModifier;
				}
				dt = hg::g_Time.UnscaledDelta() * rootRotationModifier;
			}
		}
}

void IKComponent::SetTarget(hg::Entity * target)
{
	m_target = target;
}

void IKComponent::SetSource( hg::Entity * source )
{
	m_source = source;
}

void IKComponent::SendMsg(hg::Message * msg)
{
	switch (msg->GetType())
	{
	case GameMessageType::kDeath:
		Remove(((DeathMessage*)msg)->sender->GetComponent<IKObject>());
	}
}

void IKComponent::Shutdown()
{
	for (auto* o : m_ikObjects)
	{
		if (!o->IsAlive())
			continue;

		hg::Entity* ent = o->GetEntity();
		if (!ent)
			continue;

		hg::Transform* trans = ent->GetTransform();
		if (!trans)
			continue;

		hg::Entity* particle = hg::Entity::Assemble(SID(GPUParticle_Test1));
		particle->GetTransform()->SetPosition(trans->GetWorldPosition());
		particle->GetTransform()->Rotate(trans->GetWorldRotation());
		o->GetEntity()->Destroy();
	}
	m_ikObjects.clear();
}

void IKComponent::ChainRotation()
{
	static float dt = 0;
	float r = dt;
	for (auto* o : m_ikObjects)
	{
		o->GetEntity()->GetTransform()->Rotate(0, 0, r);
		r += hg::g_Time.UnscaledDelta() * 0.01f;
	}
	dt = hg::g_Time.UnscaledDelta() * 0.1f;

}

void IKComponent::Reach()
{
	hg::Transform* entTrans = GetEntity()->GetTransform();
	m_ikObjects.back()->Drag(m_target->GetTransform());

	for (auto* o : m_ikObjects)
	{
		hg::Transform* trans = o->GetEntity()->GetTransform();
		if (o->m_parent)
		{
			hg::Transform* par_trans = o->m_parent->GetEntity()->GetTransform();
			trans->SetPosition(par_trans->GetWorldPosition() + par_trans->Forward() * o->m_length );
		}
		else
		{
			trans->SetPosition(entTrans->GetWorldPosition());
		}
	}
}

hg::IComponent * IKComponent::MakeCopyDerived() const
{
	IKComponent* copy = (IKComponent*)IComponent::Create(SID(IKComponent));
	copy->m_ikObjects.clear();
	copy->m_target = nullptr;
	copy->m_source = nullptr;
	copy->m_length = m_length;
	copy->m_numObjs = m_numObjs;
	copy->m_instanceName = m_instanceName;
	copy->m_dragSpeed = m_dragSpeed;
	copy->m_rotation = m_rotation;
	copy->m_frozen = m_frozen;
	copy->m_reversed = m_reversed;
	copy->m_iteration = m_iteration;
	copy->m_instanceInfos = m_instanceInfos;
	copy->m_boss = m_boss;
	copy->m_bossName = m_bossName;

	return copy;
}

