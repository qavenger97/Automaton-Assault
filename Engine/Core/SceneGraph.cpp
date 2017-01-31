#include <pch.h>
#include "SceneGraph.h"
#include "Entity.h"
void Hourglass::SceneGraph::Init()
{
	root = this;
	m_parent = nullptr;

	worldTransform = localTransform = TransformData::Identity();
}

void Hourglass::SceneGraph::Update()
{
	for (Transform* child : m_children)
	{
		if (child->GetChildrenCount() >= 7)
			int i = 0;
		child->CheckAndUpdate();
	}
}

void Hourglass::SceneGraph::Reset()
{
	while (m_children.size() > 0)
	{
		if (m_children.front()->GetEntity())
		{
			m_children.front()->GetEntity()->Destroy();
		}
		else
		{
			m_children.front()->Destroy();
		}
	}
}
