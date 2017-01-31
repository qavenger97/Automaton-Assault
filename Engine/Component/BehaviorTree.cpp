#include "pch.h"

#include "BehaviorTree.h"
#include "Core/ComponentFactory.h"
#include "Core\Entity.h"

namespace Hourglass
{
	uint32_t BehaviorTree::s_TypeID = ComponentFactory::GetSystemComponentID();

	IComponent* BehaviorTree::MakeCopyDerived() const
	{
		BehaviorTree* copy = (BehaviorTree*)IComponent::Create( SID(BehaviorTree) );
		copy->SetRoot( m_Root->MakeCopy() );
		copy->m_FirstTick = true;
		return copy;
	}

	void BehaviorTree::LoadFromXML( tinyxml2::XMLElement * data )
	{
		const char* type = data->Attribute( "type" );
		StrID name = WSID(type);
		m_Root = nullptr;
		m_Root = IBehavior::AssembleRoot( name );
		m_FirstTick = true;
	}

	void BehaviorTree::SetRoot( IBehavior * root )
	{
		if(m_Root)
			DestroyRoot();

		 m_Root = root;
	}

	void BehaviorTree::DestroyRoot()
	{
		m_Root->Destroy();
		m_Root = nullptr;
	}

	void BehaviorTree::Update()
	{
		if (!m_FirstTick)
			m_Root->Run( GetEntity() );
		else
			m_FirstTick = false;
	}

	void BehaviorTree::Shutdown()
	{
		IComponent::Shutdown();
		DestroyRoot();
	}
}