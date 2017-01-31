#pragma once

#include "StrID.h"
#include <unordered_map>

namespace Hourglass
{
	class Entity;
	class EntityAssembler
	{
	public:

		/**
		* Get a fully assembled entity, given the name of
		* the pre-assembled entity type
		*/
		static Entity* GetAssembledEntity( StrID name );

		/**
		* Get a fully assembled entity, give the name of
		* the pre-assembled entity type
		*	@param name - name of the assembled entity
		*	@param pos - position to place the assembled entity
		*	@param rot - rotation to orient the assembled entity
		*/
		static Entity* GetAssembledEntity( StrID name, const XMFLOAT3& pos, const XMFLOAT3& rot );

		/**
		* Get a fully assembled entity, give the name of
		* the pre-assembled entity type
		*	@param name - name of the assembled entity
		*	@param pos - position to place the assembled entity
		*	@param rot - rotation to orient the assembled entity
		*	@param scale - scale the assembled entity by this amount
		*/
		static Entity* GetAssembledEntity( StrID name, const XMFLOAT3& pos, const XMFLOAT3& rot, const XMFLOAT3& scale );

		static void Shutdown();

	protected:

		void Register( StrID name, Entity* root );

	private:

		static std::unordered_map< StrID, Entity*>* m_AssembledEntities;
	};
}