#include "GamePch.h"

#include "Gate.h"

IMPLEMENT_GAME_COMPONENT_TYPEID(Gate)

void Gate::Init()
{
	m_Status = kGate_Shut;
	m_Timer = 0.0f;
}

void Gate::LoadFromXML(tinyxml2::XMLElement* data)
{
	m_TargetOffset = Vector3(0, 0, 0);
	tinyxml2::XMLElement* xmlTargetOffset = data->FirstChildElement("targetOffset");

	if (xmlTargetOffset)
	{
		xmlTargetOffset->QueryFloatAttribute("x", &m_TargetOffset.x);
		xmlTargetOffset->QueryFloatAttribute("y", &m_TargetOffset.y);
		xmlTargetOffset->QueryFloatAttribute("z", &m_TargetOffset.z);
	}

	m_OpenTime = 0.0f;
	data->QueryFloatAttribute("openTime", &m_OpenTime);

	// TODO: load gate status from xml
}

void Gate::Update()
{
	//static const char* stateName[] =
	//{
	//	"kGate_Shut",
	//	"kGate_Opening",
	//	"kGate_Open",
	//	"kGate_Closing",
	//};

	//hg::DevTextRenderer::DrawText_WorldSpace(stateName[m_Status], GetEntity()->GetPosition());

	if (m_Status == kGate_Opening)
	{
		if (m_Timer < m_OpenTime)
		{
			float dt = min(hg::g_Time.Delta(), m_OpenTime - m_Timer);
			m_Timer += dt;
			GetEntity()->GetTransform()->Translate(m_TargetOffset / m_OpenTime * dt);

			if (FLT_EQUALS_ZERO(m_OpenTime - m_Timer))
				m_Status = kGate_Open;
		}
		else
			m_Status = kGate_Open;
	}
	else if (m_Status == kGate_Closing)
	{
		if (m_Timer > 0)
		{
			float dt = min(hg::g_Time.Delta(), m_Timer);
			m_Timer -= dt;
			GetEntity()->GetTransform()->Translate(m_TargetOffset / m_OpenTime * -dt);

			if (FLT_EQUALS_ZERO(m_Timer))
				m_Status = kGate_Shut;
		}
		else
			m_Status = kGate_Shut;
	}
}

void Gate::OnEvent(const StrID& eventName)
{
	if (eventName == SID(Open))
	{
		Open();
	}
}

hg::IComponent* Gate::MakeCopyDerived() const
{
	Gate* cpy = (Gate*)IComponent::Create(SID(Gate));

	cpy->m_TargetOffset = m_TargetOffset;
	cpy->m_OpenTime = m_OpenTime;
	cpy->m_Timer = 0.0f;
	cpy->m_Status = m_Status;

	return cpy;
}

void Gate::Open()
{
	switch (m_Status)
	{
	case kGate_Shut:
		hg::g_AudioSystem.PostEvent(AK::EVENTS::PLAY_SMALLDOOR_OPEN, GetEntity()->GetPosition());
	case kGate_Closing:
		m_Status = kGate_Opening;
		break;

	case kGate_Open:
	case kGate_Opening:
		m_Status = kGate_Closing;
		break;

	default:
		break;
	}
}
