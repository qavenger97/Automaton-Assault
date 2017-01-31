
#include "GamePch.h"
#include "LevelExit.h"

uint32_t LevelExit::s_TypeID = hg::ComponentFactory::GetGameComponentID();

void LevelExit::LoadFromXML(tinyxml2::XMLElement* data)
{
	tinyxml2::XMLElement* sizeXML = data->FirstChildElement("Size");

	if (sizeXML)
	{
		sizeXML->QueryFloatAttribute("x", &m_Size.x);
		sizeXML->QueryFloatAttribute("y", &m_Size.y);
		sizeXML->QueryFloatAttribute("z", &m_Size.z);
	}
	else
	{
		// Use a invalid size if we don't find size attribute
		m_Size = Vector3(-1, -1, -1);
	}
}

void LevelExit::Update()
{
	hg::Entity* ent = GetEntity();
	if (!ent)
		return;

	XMVECTOR pos = ent->GetTransform()->GetWorldPosition();
	XMVECTOR size = XMLoadFloat3(&m_Size);
	hg::Aabb aabb(pos - size / 2.0f, pos + size / 2.0f);

	hg::DebugRenderer::DrawAabb(aabb);

	std::vector<hg::Entity*> hitEntities = hg::g_Physics.TestBoxOverlapping(aabb);
	for (std::vector<hg::Entity*>::iterator iter = hitEntities.begin(); iter != hitEntities.end(); iter++)
	{
		if ((*iter)->GetTag() == SID(Player))
		{
			GetEntity()->GetComponent<hg::MeshRenderer>()->SetEnabled(true);
		}
	}
}

Hourglass::IComponent* LevelExit::MakeCopyDerived() const
{
	LevelExit* copy = (LevelExit*)IComponent::Create(SID(LevelExit));
	copy->m_Size = m_Size;
	return copy;
}
