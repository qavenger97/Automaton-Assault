#pragma once
#include "..\pch.h"

#include <cstdint>


#define DECLARE_COMPONENT_TYPEID					\
	public:											\
		int GetTypeID() const { return s_TypeID; }	\
		static uint32_t s_TypeID;					\
	private:

#define IMPLEMENT_COMPONENT_TYPEID(type)			\
	uint32_t type::s_TypeID = Hourglass::ComponentFactory::GetSystemComponentID();

#define IMPLEMENT_GAME_COMPONENT_TYPEID(type)		\
	uint32_t type::s_TypeID = Hourglass::ComponentFactory::GetGameComponentID();


namespace Hourglass
{
	class Entity;
	class Message;

	class IComponent
	{
	public:

		IComponent();

		virtual ~IComponent() {}

		virtual void LoadFromXML( tinyxml2::XMLElement* data ) { };

		/**
		* Create a new component, given the registered name of the component
		*/
		static IComponent* Create(StrID name);

		FORCE_INLINE Entity* GetEntity()const { return m_Entity; }

		virtual int GetTypeID() const = 0;

		virtual IComponent* MakeCopy();

		/**
		* Make a copy of this component
		*/
		virtual IComponent* MakeCopyDerived() const = 0;
		
		bool IsAlive() const { return m_IsAlive; }

		bool IsEnabled() const { return m_IsEnabled; }

		void SetEntity( Entity* entity ) { m_Entity = entity; }

		void SetEnabled( bool enabled ) { m_IsEnabled = enabled; }

		void SetAlive( bool alive ) { m_IsAlive = alive; }

		/**
		* Attaches this component to the passed in entity
		*/
		void Attach( Entity* entity );

		/**
		* Detaches this component from it's owning owning entity
		*/
		void Detach();

		/**
		* Called on frame update by the system that holds this component
		*/
		virtual void Update() { };

		/**
		* Called on initialization by the owning entity
		*/
		virtual void Init() { };

		virtual void Start() { };

		/**
		* Called just before the entity is destroyed
		*/
		virtual void Shutdown();

		/**
		* Called when component gets a message from entity
		*/
		virtual void OnMessage(Message* msg) {}

		/**
		* Called when entity collides with another entity
		*/
		virtual void OnCollision(Entity* other) {}

		virtual void OnTriggerEnter(Entity* other) {}
		virtual void OnTriggerExit(Entity* other) {}

		virtual void OnEvent(const StrID& eventName) {}
	private:

		uint32_t m_IsAlive : 1;
		uint32_t m_IsEnabled : 1;
		Entity* m_Entity;		
	};
}