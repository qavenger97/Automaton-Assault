#include "GamePch.h"

#include "Temp_PlayerDie.h"

void Temp_PlayerDie::LoadFromXML( tinyxml2::XMLElement* data )
{
	m_DeathEndTime = 1.0f;
	data->QueryFloatAttribute( "end_time", &m_DeathEndTime );

	m_FlickerFreq = 0.05f;
	data->QueryFloatAttribute( "flicker_freq", &m_FlickerFreq );
}

void Temp_PlayerDie::Init( Hourglass::Entity * entity )
{
	m_Timer = 0.0f;
	m_NextColorSwap = m_FlickerFreq;
	m_FlickerColor = Color( 0.2f, 0.2f, 0.2f, 1.0f );
	Vector3 wpos = entity->GetTransform()->GetWorldPosition();
	m_YAxisStartPos = wpos.y;
}

hg::IBehavior::Result Temp_PlayerDie::Update( Hourglass::Entity * entity )
{
	if (!m_Pr)
	{
		m_Pr = entity->GetComponent<hg::MeshRenderer>();
		if (m_Pr)
		{
			m_MeshColor = m_Pr->GetColor();
		}
		else
		{
			return kSUCCESS;
		}
	}

	if (m_Pr)
	{
		m_Timer += hg::g_Time.Delta();

		float r = m_Timer / m_DeathEndTime;
		float rPI = r * XM_PI;

		// Flick the cube up/falls back down
		Vector3 wpos = entity->GetTransform()->GetWorldPosition();
		wpos.y = m_YAxisStartPos + sinf( rPI );
		hg::Transform* trans = entity->GetTransform();
		trans->SetPosition( wpos );

		// Shrink to oblivion
		float scale = max(0,1.0f - r*r*r);
		trans->SetScale( XMFLOAT3( scale, scale, scale ) );

		if (m_Timer >= m_NextColorSwap)
		{
			m_NextColorSwap += m_FlickerFreq;

			if (m_IsFlickering)
			{
				m_Pr->SetColor( m_FlickerColor );
			}
			else
			{
				m_Pr->SetColor( m_MeshColor );
			}

			m_IsFlickering = !m_IsFlickering;
		}
	}
	return kRUNNING;
}

hg::IBehavior * Temp_PlayerDie::MakeCopy() const
{
	Temp_PlayerDie* copy = (Temp_PlayerDie*)IBehavior::Create( SID( Temp_PlayerDie ) );
	copy->m_Pr = nullptr;
	copy->m_DeathEndTime = m_DeathEndTime;
	copy->m_FlickerFreq = m_FlickerFreq;
	copy->m_FlickerColor = m_FlickerColor;
	return copy;
}
