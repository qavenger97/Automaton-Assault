#include "GamePch.h"
#include "IKComponent.h"
#include "GrapplerComp.h"
#include "GameMessage.h"

uint32_t GrapplerComp::s_TypeID = hg::ComponentFactory::GetGameComponentID();

void GrapplerComp::LoadFromXML( tinyxml2::XMLElement* data )
{
}

void GrapplerComp::SetGrapple( hg::Entity * grapple )
{
	m_Grapple = grapple;
	GetEntity()->GetComponent<IKComponent>()->SetTarget(grapple);
}

void GrapplerComp::Shutdown()
{
	if(m_Grapple)
		m_Grapple->Destroy();

	hg::g_AudioSystem.SetState(AK::STATES::ENEMY::GROUP, AK::STATES::ENEMY::STATE::DEFAULT);
}

void GrapplerComp::OnMessage(hg::Message* msg)
{
	switch (msg->GetType())
	{
		case GameMessageType::kDeath:
			// Stop playing sounds on grappler
			hg::g_AudioSystem.PostEvent(AK::EVENTS::STOP_GRAPPLER_EXTENDCLAW);
			hg::g_AudioSystem.PostEvent(AK::EVENTS::STOP_GRAPPLER_REELIN);
			break;
	}
}

hg::IComponent* GrapplerComp::MakeCopyDerived() const
{
	GrapplerComp* copy = (GrapplerComp*)IComponent::Create( SID(GrapplerComp) );

	copy->m_Grapple = nullptr;
	copy->m_GrappleStartPos = m_GrappleStartPos;
	copy->m_RadialPatrolStartPoint = m_RadialPatrolStartPoint;

	return copy;
}
