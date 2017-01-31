#include "GamePch.h"

#include "AI_Die.h"
#include "GameMessage.h"

extern bool g_bDebugAIBehaviors;

void AI_Die::LoadFromXML( tinyxml2::XMLElement* data )
{
	m_DeathEndTime = 1.0f;
	data->QueryFloatAttribute( "end_time", &m_DeathEndTime );

	m_AnimPauseTime = 0.0f;
	data->QueryFloatAttribute( "pause_time", &m_AnimPauseTime );

	const char* deathAnim = data->Attribute( "death_anim" );
	if (deathAnim)
		m_DeathAnim = WSID( deathAnim );
	else
		m_DeathAnim = WSID("");

	bool explode = false;
	data->QueryBoolAttribute( "explode", &explode );
	m_ExplodeOnDeath = explode;

	m_Anim = nullptr;
	m_dropLoot = 0;
	data->QueryFloatAttribute("loot", &m_dropLoot);
}

void AI_Die::Init( Hourglass::Entity * entity )
{
	m_Timer = 0.0f;

	hg::Transform& trans = *entity->GetTransform();
	Vector3 wpos = trans.GetWorldPosition();
	m_StartScale = trans.GetWorldScale();
	m_YAxisStartPos = wpos.y;
	if (m_Anim == nullptr)
	{
		m_Anim = entity->GetComponent<hg::Animation>();
	}

	hg::Motor* motor = entity->GetComponent<hg::Motor>();
	if(motor)
		motor->SetMoveEnabled( false );

	if(m_Anim)
		m_Anim->Play( m_DeathAnim, 1.0f );

	entity->GetComponent<hg::DynamicCollider>()->SetEnabled( false );

	if (m_ExplodeOnDeath)
	{
		// Play explosion effect
		Vector3 pos = entity->GetPosition();
		hg::Entity::Assemble(SID(FX_Explosion), pos, Quaternion::Identity);

		hg::g_AudioSystem.PostEvent(AK::EVENTS::PLAY_SPIDERBOT_EXPLODE, entity->GetPosition());
	}
}

hg::IBehavior::Result AI_Die::Update( Hourglass::Entity * entity )
{
	if (g_bDebugAIBehaviors)
	{
		hg::DevTextRenderer::DrawText_WorldSpace("AI_Die", entity->GetPosition());
	}
	if (m_Anim)
	{
		if (m_Anim->IsEnding())
		{
			m_Anim->Pause();
		}
	}
	
	m_Timer += hg::g_Time.Delta();
	if (m_Timer > m_AnimPauseTime)
	{
		
		/*if(m_Anim)
			m_Anim->Pause();*/
		/*float r = (m_Timer - m_AnimPauseTime) / (m_DeathEndTime - m_AnimPauseTime);
		float rPI = r * XM_PI;*/

		// Flick him up / falls back down
		Vector3 wpos = entity->GetTransform()->GetWorldPosition();
		//wpos.y = m_YAxisStartPos + sinf( rPI );
		hg::Transform* trans = entity->GetTransform();
		trans->SetPosition( wpos );

		//// Shrink to oblivion
		//float scale = (1.0f - r*r*r*r);
		//trans->SetScale( m_StartScale * scale );

		if (m_Timer > m_DeathEndTime)
		{
			
			/*if(m_Anim)
				m_Anim->Resume();*/

			entity->GetComponent<hg::DynamicCollider>()->SetEnabled( true );
			if (m_dropLoot>0)
			{
				static StrID items[] = { { SID(MedPack) }, SID(AmmoPack), SID(Pickup_Ricochet), SID(Pickup_Shield) };
				float chance = Math::RNG::Range(0.0f, 1.0f);
				if (chance <= m_dropLoot)
				{
					int rnd = Math::RNG::Range(0, 99);
					XMVECTOR pos = entity->GetTransform()->GetWorldPosition();
					int i = ((rnd <= 49) << 1) + (rnd & 1);
					hg::Entity::Assemble(items[i], XMVectorSetY(pos, 1), entity->GetTransform()->GetWorldRotation());
				}
			}
			entity->Destroy();
		}
	}
	m_Timer += hg::g_Time.Delta();

	
	return kRUNNING;
}

hg::IBehavior * AI_Die::MakeCopy() const
{
	AI_Die* copy = (AI_Die*)IBehavior::Create( SID( AI_Die ) );
	copy->m_DeathEndTime = m_DeathEndTime;
	copy->m_Anim = nullptr;
	copy->m_DeathAnim = m_DeathAnim;
	copy->m_DeathEndTime = m_DeathEndTime;
	copy->m_AnimPauseTime = m_AnimPauseTime;
	copy->m_ExplodeOnDeath = m_ExplodeOnDeath;
	copy->m_dropLoot = m_dropLoot;
	return copy;
}
