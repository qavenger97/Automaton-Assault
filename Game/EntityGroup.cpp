#include "GamePch.h"

#include "EntityGroup.h"

uint32_t EntityGroup::s_TypeID = hg::ComponentFactory::GetGameComponentID();

void EntityGroup::Start()
{
	m_bAloneState = true;
	m_Entities.clear();
}

void EntityGroup::Update()
{
	// Change music atmosphere based on enemies nearby
	if (IsAlone())
	{
		if (!m_bAloneState)
		{
			hg::g_AudioSystem.SetState(AK::STATES::ATMOSPHERE::GROUP, AK::STATES::ATMOSPHERE::STATE::SAFE);
			m_bAloneState = true;
		}
	}
	else
	{
		if (m_bAloneState)
		{
			hg::g_AudioSystem.SetState(AK::STATES::ATMOSPHERE::GROUP, AK::STATES::ATMOSPHERE::STATE::DANGER);
			m_bAloneState = false;
		}
	}

	//char buf[1024];
	//sprintf_s(buf, "IsAlone: %d, alone state: %d\n", IsAlone(), m_bAloneState);
	//hg::DevTextRenderer::DrawText_WorldSpace(buf, GetEntity()->GetTransform()->GetWorldPosition());

}

hg::IComponent* EntityGroup::MakeCopyDerived() const
{
	EntityGroup* copy = (EntityGroup*)IComponent::Create( SID(EntityGroup) );
	copy->m_Entities.clear();
	return copy;
}
