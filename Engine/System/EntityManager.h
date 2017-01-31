#pragma once

#include "Core\Entity.h"
#include "Core\StrID.h"

namespace Hourglass
{
	class EntityManager
	{
	public:

		void Init();

		void Shutdown();

		/**
		* Get an available entity
		*/
		Entity* GetFree();

		/**
		* Returns the First entity found with a given name
		*	@param name - StrID that is serached for
		*/
		Entity* FindByName( StrID name );

		/**
		* Returns the First entity found with a given tag
		*	@param tag - StrID that is serached for
		*/
		Entity* FindByTag( StrID tag );

		Entity* FindByInstanceID( int instanceId );

		/**
		* Handles the destruction of this entity
		*   @param entity - The entity to destroy
		*/
		void DestroyEntity( Entity* entity );

		/**
		* Loads an entity from xml data
		*/
		void LoadFromXML( Entity* entity, tinyxml2::XMLElement* element );

		/**
		* Loads an assembled entity from xml data
		*/
		void CreateAssembledFromXML( tinyxml2::XMLElement* element );

		void ResetEntities();

		void BroadcastMessage(hg::Message* msg);

	private:

		static const unsigned int s_kMaxEntities = 4096;
		Entity m_Entities[s_kMaxEntities];

		std::vector<Entity*> m_AssembledEntities;
	};

	extern EntityManager g_EntityManager;
}