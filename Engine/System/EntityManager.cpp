#include "pch.h"

#include "EntityManager.h"
#include "Core\ComponentFactory.h"
#include "Core\AssembledEntityReg.h"
#include "Component\Animation.h"

namespace Hourglass
{
	EntityManager g_EntityManager;

	void EntityManager::Init()
	{
		for (unsigned int i = 0; i < s_kMaxEntities; ++i)
		{
			m_Entities[i].GetTransform()->Init();
		}
	}

	void EntityManager::Shutdown()
	{
		for (unsigned int i = 0; i < m_AssembledEntities.size(); ++i)
		{
			Animation* anim = m_AssembledEntities[i]->GetComponent<Animation>();
			if (anim)
			{
				anim->Shutdown();
			}
			delete m_AssembledEntities[i];
		}
		
		for (unsigned int i = 0; i < s_kMaxEntities; ++i)
		{
			m_Entities[i].GetTransform()->Shutdown();
		}
	}

	Entity* EntityManager::GetFree()
	{
		for (unsigned int i = 0; i < s_kMaxEntities; ++i)
		{
			if (!m_Entities[i].IsActive())
			{
				m_Entities[i].SetActive( true );
				m_Entities[i].GetTransform()->Init();
				m_Entities[i].GetTransform()->Reset();
				m_Entities[i].Init();
				return &m_Entities[i];
			}
		}

		assert( 1 == 0 ); // we shouldn't get here ever
		return nullptr;
	}

	Entity* EntityManager::FindByName( StrID name )
	{
		for (unsigned int i = 0; i < s_kMaxEntities; ++i)
		{
			if (m_Entities[i].GetName() == name)
			{
				return &m_Entities[i];
			}
		}
		
		return nullptr;
	}

	Entity* EntityManager::FindByTag( StrID tag )
	{
		for (unsigned int i = 0; i < s_kMaxEntities; ++i)
		{
			if (m_Entities[i].GetTag() == tag)
			{
				return &m_Entities[i];
			}
		}

		return nullptr;
	}

	Entity* EntityManager::FindByInstanceID(int instanceId)
	{
		for (unsigned int i = 0; i < s_kMaxEntities; ++i)
		{
			if (m_Entities[i].GetInstanceID() == instanceId)
			{
				return &m_Entities[i];
			}
		}

		return nullptr;
	}

	void EntityManager::DestroyEntity(Entity * entity)
	{
		assert( entity != nullptr );
		entity->Shutdown();
		entity->DetachComponents();
	}

	void EntityManager::LoadFromXML( Entity* entity, tinyxml2::XMLElement* entityXML )
	{		
		// Get the entity's name and tag
		const char* entityName = entityXML->Attribute( "name" );
		const char* entityTag = entityXML->Attribute( "tag" );

		if (entityName)
		{
			entity->SetName( WSID( entityName ) );
		}
		if (entityTag)
		{
			entity->SetTag( WSID( entityTag ) );
		}

		int instanceId = 0;
		if (entityXML->QueryIntAttribute("instanceId", &instanceId) == tinyxml2::XML_SUCCESS)
		{
			entity->SetInstanceID(instanceId);
		}

		// Apply entity transform
		tinyxml2::XMLElement* transformXML = entityXML->FirstChildElement("Transform");
		if (transformXML != nullptr)
		{
			XMFLOAT3 position = { 0.0f, 0.0f, 0.0f };
			float pitch = 0, yaw = 0, roll = 0;
			XMFLOAT3 scale = { 1.0f, 1.0f, 1.0f };

			// Apply position
			tinyxml2::XMLElement* positionXML = transformXML->FirstChildElement( "Position" );
			if (positionXML != nullptr)
			{
				positionXML->QueryFloatAttribute( "x", &position.x );
				positionXML->QueryFloatAttribute( "y", &position.y );
				positionXML->QueryFloatAttribute( "z", &position.z );
			}

			// Apply rotation
			tinyxml2::XMLElement* rotationXML = transformXML->FirstChildElement( "Rotation" );
			if (rotationXML != nullptr)
			{
				rotationXML->QueryFloatAttribute( "pitch", &pitch );
				rotationXML->QueryFloatAttribute( "yaw", &yaw );
				rotationXML->QueryFloatAttribute( "roll", &roll );
			}

			// Apply scale
			tinyxml2::XMLElement* scaleXML = transformXML->FirstChildElement( "Scale" );
			if (scaleXML != nullptr)
			{
				scaleXML->QueryFloatAttribute( "x", &scale.x );
				scaleXML->QueryFloatAttribute( "y", &scale.y );
				scaleXML->QueryFloatAttribute( "z", &scale.z );
			}

			Transform* trans = entity->GetTransform();

			trans->Rotate( pitch, yaw, roll );
			trans->Scale(scale); 
			trans->Translate( position );
		}

		// Add Components to the entity, as defined in the XML file, letting individual components handle their own attributes
		tinyxml2::XMLElement* componentsXML = entityXML->FirstChildElement( "Components" );
		if (!componentsXML)
		{
			return;
		}

		tinyxml2::XMLElement* componentXML = componentsXML->FirstChildElement();
		while (componentXML)
		{				
			// Retrieve a new component from the pool
			const char* componentName = componentXML->Value();

			if (ComponentFactory::IsComponentTypeRegistered( WSID( componentName ) ))
			{
				IComponent* component = IComponent::Create( WSID( componentName ) );
			
				// Let the individual component handle its own XML attributes
				component->LoadFromXML( componentXML );
				entity->AttachComponent( component );

				// Handle attributes that every component will have
				bool enabled = true;
				componentXML->QueryBoolAttribute( "enabled", &enabled );
				component->SetEnabled( enabled );
			}
			else
			{
				char buf[1024];
				sprintf_s(buf, "Warning: Unable to create component type \'%s\'.\n", componentName);
				OutputDebugStringA(buf);
			}

			componentXML = componentXML->NextSiblingElement();
		}

		tinyxml2::XMLElement* childrenXML = entityXML->FirstChildElement("Children");
		if (childrenXML)
		{
			tinyxml2::XMLElement* childXML = childrenXML->FirstChildElement();
			while (childXML)
			{
				const char* childType = childXML->Value();
				Entity* childEntity;

				if (strcmp(childType, "Entity") == 0)
				{
					childEntity = Entity::Create();
				}
				else if (strcmp(childType, "Assembled") == 0)
				{
					const char* assembledName = childXML->Attribute("name");
					childEntity = Entity::Assemble(WSID(assembledName));
				}
				
				Transform* childTransform = childEntity->GetTransform();
				childTransform->LinkTo(entity->GetTransform(), false);
				//childTransform->UpdateTransform();
				LoadFromXML(childEntity, childXML);
				childXML = childXML->NextSiblingElement();
			}
		}

		entity->Init();
	}
	
	void EntityManager::CreateAssembledFromXML( tinyxml2::XMLElement* entityXML )
	{
		Entity* entity = new Entity;

		// Get the assembled name and tag
		const char* assembledName = entityXML->Attribute( "name" );
		const char* assembledTag = entityXML->Attribute( "tag" );

		assert( assembledName ); // must have a name; nameless assembled type makes no sense.
		if (assembledName)
		{
			entity->SetName( WSID( assembledName ) );
		}
		if (assembledTag)
		{
			entity->SetTag( WSID( assembledTag ) );
		}

		// Apply entity transform
		tinyxml2::XMLElement* transformXML = entityXML->FirstChildElement( "Transform" );
		if (transformXML != nullptr)
		{
			XMFLOAT3 position = { 0.0f, 0.0f, 0.0f };
			float pitch = 0, yaw = 0, roll = 0;
			XMFLOAT3 scale = { 1.0f, 1.0f, 1.0f };

			// Apply position
			tinyxml2::XMLElement* positionXML = transformXML->FirstChildElement( "Position" );
			if (positionXML != nullptr)
			{
				positionXML->QueryFloatAttribute( "x", &position.x );
				positionXML->QueryFloatAttribute( "y", &position.y );
				positionXML->QueryFloatAttribute( "z", &position.z );
			}

			// Apply rotation
			tinyxml2::XMLElement* rotationXML = transformXML->FirstChildElement( "Rotation" );
			if (rotationXML != nullptr)
			{
				rotationXML->QueryFloatAttribute( "pitch", &pitch );
				rotationXML->QueryFloatAttribute( "yaw", &yaw );
				rotationXML->QueryFloatAttribute( "roll", &roll );
			}

			// Apply scale
			tinyxml2::XMLElement* scaleXML = transformXML->FirstChildElement( "Scale" );
			if (scaleXML != nullptr)
			{
				scaleXML->QueryFloatAttribute( "x", &scale.x );
				scaleXML->QueryFloatAttribute( "y", &scale.y );
				scaleXML->QueryFloatAttribute( "z", &scale.z );
			}

			Transform* trans = entity->GetTransform();
			trans->SetScale( scale );
			trans->SetRotation( pitch, yaw, roll );
			trans->SetPosition( position);
		}

		// Add Components to the entity, as defined in the XML file, letting individual components handle their own attributes
		tinyxml2::XMLElement* componentsXML = entityXML->FirstChildElement( "Components" );

		if (!componentsXML)
		{
			return;
		}

		tinyxml2::XMLElement* componentXML = componentsXML->FirstChildElement();
		while (componentXML)
		{
			// Retrieve a new component from the pool
			const char* componentName = componentXML->Value();
			
			if (ComponentFactory::IsComponentTypeRegistered( WSID( componentName ) ))
			{
				IComponent* component = ComponentFactory::GetComponentToAssemble( WSID( componentName ) );

				// Let the individual component handle its own XML attributes
				component->LoadFromXML( componentXML );
				entity->AttachComponent( component );

				// Handle attributes that every component will have
				bool enabled = true;
				componentXML->QueryBoolAttribute( "enabled", &enabled );
				component->SetAlive( true );
				component->SetEnabled( enabled );
			}
			else
			{
				char buf[1024];
				sprintf_s(buf, "Warning: Unable to create component type \'%s\'.\n", componentName);
				OutputDebugStringA(buf);
			}

			componentXML = componentXML->NextSiblingElement();
		}

		m_AssembledEntities.push_back( entity );
		hg::AssembledEntityReg reg( WSID( assembledName ), entity );
	}
	void EntityManager::ResetEntities()
	{
		for (int i = 0; i < s_kMaxEntities; i++)
		{
			m_Entities[i].GetTransform()->Init();
		}
	}
	void EntityManager::BroadcastMessage(hg::Message * msg)
	{
		for (uint32_t i = 0; i < s_kMaxEntities; i++)
		{
			if (m_Entities[i].IsActive())
			{
				m_Entities[i].SendMsg(msg);
			}
		}
	}
}