#pragma once

#include "Component\IComponent.h"
#include <string>
#include "Component\Transform.h"
#include "StrID.h"

namespace Hourglass
{
	class Message;

	class Entity
	{
	public:

		Entity();

		~Entity();

		StrID GetName() const { return m_Name; }

		StrID GetTag() const { return m_Tag; }

		int GetInstanceID() const { return m_InstanceID; }

		bool IsActive() const { return m_IsActive; }

		bool IsEnabled() const { return m_IsEnabled; }

		/**
		* Get an available entity
		*/
		static Entity* Create();

		/**
		* Returns the First entity found with a given name
		*	@param name - StrID that is serached for
		*/
		static Entity* FindByName( StrID name );

		/**
		* Returns the First entity found with a given tag
		*	@param tag - StrID that is serached for
		*/
		static Entity* FindByTag( StrID tag );

		static Entity* FindByInstanceID( int instanceId );

		/**
		* Makes a copy of a pre-assembled entity
		*	@param assembleName- StrID of the assembled entity to copy
		*/
		static Entity* Assemble( StrID assembleName , FXMVECTOR position = XMVectorZero(), FXMVECTOR rotation = XMQuaternionIdentity());

		/**
		* Make a Copy of this entity
		*/
		Entity* MakeCopy() const;

		/**
		* Make a Copy of this entity
		*	@param pos - position to place the copy
		*	@param rot = rotation to place the copy
		*/
		Entity* MakeCopy(const XMFLOAT3& pos, const XMFLOAT3& rot) const;

		/**
		* Make a Copy of this entity
		*	@param pos - position to place the copy
		*	@param rot - rotation to place the copy
		*	@param scale - set the scale of the copy
		*/
		Entity* MakeCopy( const XMFLOAT3& pos, const XMFLOAT3& rot, const XMFLOAT3& scale) const;

		/**
		* Destroys this entity
		*/
		void Destroy();

		/**
		* Get the first component that has the corresponding type id
		*/
		IComponent* GetComponent( unsigned int id ) const;

		template<typename T>
		T* GetComponent() const { return static_cast<T*>(GetComponent( T::s_TypeID )); }

		// Get list of a type of component in all children
		template<typename T>
		std::vector<T*> GetComponentsInChildren(bool includeSelf=true) const
		{
			std::vector<T*> result;

			if (includeSelf)
			{
				T* comp = GetComponent<T>();

				if (comp)
					result.push_back(comp);
			}

			for (uint32_t i = 0; i < transform.GetChildrenCount(); i++)
			{
				Transform* child = transform.GetChild(i);
				std::vector<T*> childResult = child->GetEntity()->GetComponentsInChildren<T>();
				result.insert(result.end(), childResult.begin(), childResult.end());
			}

			return result;
		}

		void SetName( StrID name ) { m_Name = name; }

		void SetTag( StrID tag ) { m_Tag = tag; }

		void SetInstanceID( int instanceId ) { m_InstanceID = instanceId; }

		/**
		* Add a component to this entity (created and attached)
		*/
		IComponent* AddComponent( StrID name );

		void RemoveComponent( IComponent* component );

		/**
		* Add a component to this entity (created and attached)
		*/
		void AttachComponent( IComponent* entity );

		/**
		* Detach all components from this entity
		*/
		void DetachComponents();

		void SetActive( bool active ) { m_IsActive = active; }

		void SetEnabled( bool enabled ) { m_IsEnabled = enabled; }

		Entity* GetParent() const;

		/**
		* Call init on all of the components on this entity
		*/
		void Init();

		/**
		* Call shutdown on all of the components on this entity, clean up
		*/
		void Shutdown();

		/**
		* This handles any messages sent by going through the game logic
		*/
		void SendMsg( Message* msg );

		Transform* GetTransform();

		const Transform* GetTransform() const;

		// Get world position of this entity
		Vector3 GetPosition() const;

	private:

		StrID m_Name;
		StrID m_Tag;
		
		Transform transform;

		int		m_InstanceID;
		uint8_t m_IsActive : 1;
		uint8_t m_IsInitialized : 1;
		uint8_t m_IsEnabled : 1;
		uint8_t m_NumComponents;

		static const uint32_t s_kMaxComponents = 36;        // TODO: Tune this number if we stick with using this data
		IComponent* m_Components[s_kMaxComponents];			// All one array for now, we may separate game logic later [Brandon 10/1/16]
	};
}