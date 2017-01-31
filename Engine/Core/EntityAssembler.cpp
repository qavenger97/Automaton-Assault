#include "pch.h"

#include "EntityAssembler.h"
#include "Entity.h"

std::unordered_map< StrID, Hourglass::Entity*>* Hourglass::EntityAssembler::m_AssembledEntities = nullptr;

Hourglass::Entity * Hourglass::EntityAssembler::GetAssembledEntity( StrID name )
{
	std::unordered_map<StrID, Hourglass::Entity*>::iterator it =
		m_AssembledEntities->find( name );

	assert( it != m_AssembledEntities->end() );

	if (it == m_AssembledEntities->end())
	{
		return nullptr;
	}

	return it->second->MakeCopy();
}

Hourglass::Entity * Hourglass::EntityAssembler::GetAssembledEntity( StrID name, const XMFLOAT3 & pos, const XMFLOAT3 & rot )
{
	std::unordered_map<StrID, Hourglass::Entity*>::iterator it =
		m_AssembledEntities->find( name );

	assert( it != m_AssembledEntities->end() );

	if (it == m_AssembledEntities->end())
	{
		return nullptr;
	}

	return it->second->MakeCopy(pos, rot);
}

Hourglass::Entity * Hourglass::EntityAssembler::GetAssembledEntity( StrID name, const XMFLOAT3 & pos, const XMFLOAT3 & rot, const XMFLOAT3 & scale )
{
	std::unordered_map<StrID, Hourglass::Entity*>::iterator it =
		m_AssembledEntities->find( name );

	assert( it != m_AssembledEntities->end() );

	if (it == m_AssembledEntities->end())
	{
		return nullptr;
	}

	return it->second->MakeCopy( pos, rot, scale );
}

void Hourglass::EntityAssembler::Shutdown()
{
	delete m_AssembledEntities;
}

void Hourglass::EntityAssembler::Register( StrID name, Entity* root )
{
	if (m_AssembledEntities == nullptr)
	{
		m_AssembledEntities = new std::unordered_map<StrID, Entity*>();
	}

	m_AssembledEntities->insert( std::make_pair( name, root ) );
}
