#include "pch.h"

#include "Entity.h"
#include "System\EntityManager.h"
#include "ComponentFactory.h"
#include "EntityAssembler.h"
#include "Message.h"
#include "System\PhysicsSystem.h"
#include "Component\TriggerEventGroup.h"

namespace Hourglass
{
#define UNASSIGNED_INSTANCE_ID 0

	Entity::Entity()
		:
		transform(this)
	{
		for (unsigned int comp = 0; comp < s_kMaxComponents; ++comp)
		{
			m_Components[comp] = nullptr;
		}

		m_IsActive = m_IsInitialized = false;
		m_NumComponents = 0;
		m_InstanceID = UNASSIGNED_INSTANCE_ID;
	}

	Entity::~Entity()
	{
		for (unsigned int i = 0; i < m_NumComponents; ++i)
		{
			m_Components[i] = nullptr;
		}
	}

	/**
	* Get an available entity
	*/
	Entity* Entity::Create()
	{
		return g_EntityManager.GetFree();
	}

	/**
	* Returns the First entity found with a given name
	*	@param name - StrID that is serached for
	*/
	Entity* Entity::FindByName( StrID name )
	{
		return g_EntityManager.FindByName( name );
	}

	/**
	* Returns the First entity found with a given tag
	*	@param tag - StrID that is serached for
	*/
	Entity* Entity::FindByTag( StrID tag )
	{
		return g_EntityManager.FindByTag( tag );
	}

	Entity* Entity::FindByInstanceID(int instanceId)
	{
		return g_EntityManager.FindByInstanceID( instanceId );
	}

	Entity* Entity::Assemble(StrID assembleName, FXMVECTOR position /*= XMVectorZero() */, FXMVECTOR rotation /*= XMQuaternionIdentity() */)
	{
		Entity* ent = EntityAssembler::GetAssembledEntity( assembleName );
		if (ent)
		{
			ent->GetTransform()->SetPosition(position);
			ent->GetTransform()->SetRotation(rotation);
			for (unsigned int i = 0; i < ent->m_NumComponents; ++i)
			{
				ent->m_Components[i]->Start();
			}
		}
		return ent;
	}

	Entity* Entity::MakeCopy() const
	{
		Entity* copy = Create();

		const Transform* trans = GetTransform();
		Transform* copyTrans = copy->GetTransform();

		copyTrans->SetPosition( trans->GetWorldRotation() );
		copyTrans->SetRotation( trans->GetWorldRotation() );
		copyTrans->SetScale( trans->GetWorldScale() );

		copy->m_Name = m_Name;
		copy->m_Tag = m_Tag;
		copy->m_InstanceID = UNASSIGNED_INSTANCE_ID;

		for (unsigned int i = 0; i < m_NumComponents; ++i)
		{
			copy->AttachComponent(m_Components[i]->MakeCopy());
		}

		// Copy children, attach them as well
		unsigned int numChildren = trans->GetChildrenCount();
		for (unsigned int i = 0; i < numChildren; ++i)
		{
			Transform* childTrans = trans->GetChild( i );
			assert( childTrans != nullptr );
			Entity* child = childTrans->GetEntity();
			assert( child != nullptr );
			Entity* childCopy = child->MakeCopy(); // Process of MakeCopy() now starts for child
			childCopy->GetTransform()->LinkTo( copyTrans );
		}

		copy->Init();
		
		return copy;
	}

	Entity* Entity::MakeCopy(const XMFLOAT3& pos, const XMFLOAT3& rot) const
	{
		Entity* copy = MakeCopy();
		Transform* trans = copy->GetTransform();
		trans->SetPosition( pos );
		trans->SetRotation( rot.x, rot.y, rot.z );
		return copy;
	}

	Entity * Entity::MakeCopy( const XMFLOAT3& pos, const XMFLOAT3& rot, const XMFLOAT3& scale ) const
	{
		Entity* copy = MakeCopy( pos, rot );
		copy->GetTransform()->SetScale( scale );
		return copy;
	}

	/**
	* Destroys this entity
	*/
	void Entity::Destroy()
	{
		while(transform.GetChildrenCount()>0)
		{
			transform.GetChild(0)->GetEntity()->Destroy();
		}
		g_EntityManager.DestroyEntity( this );
	}

	/**
	* Get the first component that has the corresponding type id
	*/
	IComponent* Entity::GetComponent( unsigned int id ) const
	{
		for (unsigned int comp = 0; comp < m_NumComponents; ++comp)
		{
			if (m_Components[comp] && m_Components[comp]->GetTypeID() == id)
			{
				return m_Components[comp];
			}
		}
		return nullptr;
	}

	/**
	* Attach a component to this entity
	*/
	void Entity::AttachComponent( IComponent* component )
	{
		assert( component );

#if _DEBUG
		// we want to be sure we aren't attaching anything that is already attached
		for (unsigned int comp = 0; comp < s_kMaxComponents; ++comp)
		{
			assert( m_Components[comp] != component );
		}
#endif

		for (unsigned int comp = 0; comp < s_kMaxComponents; ++comp)
		{
			if (m_Components[comp] == nullptr)
			{
				m_Components[comp] = component;

				++m_NumComponents;

				component->SetEntity( this );

				if (m_IsInitialized)
				{
					component->Init();
				}

				return;
			}			
		}
	}

	/**
	* Attach a component to this entity
	*	@param name of the component to add
	*/
	IComponent* Entity::AddComponent( StrID name )
	{
		IComponent* component = ComponentFactory::GetFreeComponent( name );
		AttachComponent(component);
		return component;
	}	

	void Entity::RemoveComponent( IComponent* component )
	{
		for (unsigned int comp = 0; comp < s_kMaxComponents; ++comp)
		{
			if (m_Components[comp] == component)
			{
				m_Components[comp]->Detach();
				m_Components[comp]->SetAlive( false );
				
				for (unsigned int shiftingComp = comp; shiftingComp < s_kMaxComponents - 1; ++shiftingComp)
				{
					m_Components[shiftingComp] = m_Components[shiftingComp + 1];
				}

				--m_NumComponents;
				--comp;
			}
		}
	}

	/**
	* Detach all components from this entity
	*/
	void Entity::DetachComponents()
	{
		for (unsigned int comp = 0; comp < m_NumComponents; ++comp)
		{
			m_Components[comp]->Detach();
			m_Components[comp]->SetAlive( false );
			m_Components[comp] = nullptr;
		}

		m_NumComponents = 0;
	}

	Hourglass::Entity* Entity::GetParent() const
	{
		Transform* parent = transform.GetParent();

		if (!parent)
			return nullptr;

		return parent->GetEntity();
	}

	/**
	* Call init on all of the components on this entity
	*/
	void Entity::Init()
	{
		for (unsigned int comp = 0; comp < m_NumComponents; ++comp)
		{
			m_Components[comp]->Init();
		}
		
		m_IsInitialized = true;
		m_IsActive = true;
	}

	/**
	* Call shutdown on all of the components on this entity, clean up
	*/
	void Entity::Shutdown()
	{
		for (unsigned int comp = 0; comp < m_NumComponents; ++comp)
		{
			m_Components[comp]->Shutdown();
		}
		transform.UnLink(false);
		m_IsInitialized = false;
		m_IsActive = false;
	}

	/**
	* This handles any messages sent by going through the game logic
	*/
	void Entity::SendMsg( Message* msg )
	{
		switch (msg->GetType())
		{
		case MessageType::kCollision:
			{
				for (unsigned int comp = 0; comp < m_NumComponents; ++comp)
				{
					m_Components[comp]->OnCollision(static_cast<CollisionMessage*>(msg)->otherEntity);
				}
			}
			break;

		case MessageType::kOnTriggerEnter:
			{
				for (unsigned int comp = 0; comp < m_NumComponents; ++comp)
				{
					m_Components[comp]->OnTriggerEnter(static_cast<TriggerEnterMessage*>(msg)->sourceEntity);
				}
			}
			break;

		case MessageType::kOnTriggerExit:
			{
				for (unsigned int comp = 0; comp < m_NumComponents; ++comp)
				{
					m_Components[comp]->OnTriggerExit(static_cast<TriggerExitMessage*>(msg)->sourceEntity);
				}
			}
			break;

		case MessageType::kOnEvent:
			{
				EventMessage* e = static_cast<EventMessage*>(msg);

				if (e->eventName == SID(Kill))
				{
					Destroy();
				}
				else
				{
					for (unsigned int comp = 0; comp < m_NumComponents; ++comp)
					{
						m_Components[comp]->OnEvent(e->eventName);
					}
				}
			}
			break;

		default:
			{
				// Any messages not handled by entity will be forward to components
				for (unsigned int comp = 0; comp < m_NumComponents; ++comp)
				{
					m_Components[comp]->OnMessage(msg);
				}
			}
			break;
		}
	}

	Transform * Entity::GetTransform()
	{
		return &transform;
	}

	const Transform * Entity::GetTransform() const
	{
		return &transform;
	}

	DirectX::SimpleMath::Vector3 Entity::GetPosition() const
	{
		return transform.GetWorldPosition();
	}

}