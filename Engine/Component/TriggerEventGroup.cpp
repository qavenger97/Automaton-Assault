/*!
 * \file TriggerEventGroup.cpp
 *
 * \author Shiyang Ao
 * \date November 2016
 *
 * 
 */

#include "pch.h"

#include "TriggerEventGroup.h"
#include "Core\ComponentFactory.h"
#include "Core\Entity.h"

IMPLEMENT_COMPONENT_TYPEID(Hourglass::TriggerEventGroup)

Hourglass::IComponent* Hourglass::TriggerEventGroup::MakeCopyDerived() const
{
	TriggerEventGroup* copy = (TriggerEventGroup*)IComponent::Create(SID(TriggerEventGroup));
	return copy;
}

void Hourglass::TriggerEventGroup::OnTriggerEnter(Entity* other)
{
	// TODO: handle other type of entities later
	if (other->GetTag() != SID(Player))
		return;

	FireEvents();
}
