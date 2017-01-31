#include "GamePch.h"

#include "AI_Lunge.h"
#include "UnitShield.h"
#include "ChargerComp.h"

void AI_Lunge::LoadFromXML( tinyxml2::XMLElement* data )
{
	const char* animName = data->Attribute( "anim" );
	m_Anim = WSID(animName);

	m_Dist = 2.0f;
	data->QueryFloatAttribute( "dist", &m_Dist );

	m_Speed = 20.0f;
	data->QueryFloatAttribute( "speed", &m_Speed );

	data->QueryFloatAttribute( "shield_r", &m_ShieldColor.x );
	data->QueryFloatAttribute( "shield_g", &m_ShieldColor.y );
	data->QueryFloatAttribute( "shield_b", &m_ShieldColor.z );

	data->QueryFloatAttribute( "shadow_r", &m_ShadowColor.x );
	data->QueryFloatAttribute( "shadow_g", &m_ShadowColor.y );
	data->QueryFloatAttribute( "shadow_b", &m_ShadowColor.z );

	m_NumShadows = 5;
	data->QueryUnsignedAttribute( "shadow_count", &m_NumShadows );

	m_ShadowLifetime = 0.1f;	
	data->QueryFloatAttribute( "shadow_lifetime", &m_ShadowLifetime );

	m_RecoverLength = 0.5f;
	data->QueryFloatAttribute( "recover_length", &m_RecoverLength );

	bool inverted = false;
	data->QueryBoolAttribute( "inverted", &inverted );
	m_Inverted = inverted;
}

void AI_Lunge::Init( Hourglass::Entity* entity )
{
	hg::Motor* motor = entity->GetComponent<hg::Motor>();

	hg::Transform* trans = entity->GetTransform();
	
	motor->SetEnabled( true );
	motor->SetMoveEnabled( true );
	if(!m_Inverted)
		motor->SetDesiredMove( -trans->Forward() );
	motor->SetSpeed( m_Speed );

	UnitShield* us = entity->GetComponent<UnitShield>();
	m_ShadowRadius = us->GetRadius();
	m_ShadowSegments = us->GetMaxSengments();
	m_LungeStart = trans->GetWorldPosition();
	m_LungeStart += us->GetOffset();
	us->SetEnabled( false ); // letting shadow copies handle the visuals of the shield

	entity->GetComponent<hg::Separation>()->SetEnabled( false );

	entity->GetComponent<ChargerComp>()->SetLunging( true );

	entity->GetComponent<hg::Animation>()->Play( m_Anim, 1.0 );
	m_Traveled = 0.0f;
	m_RecoverTimer = 0.0f;
}

void AI_Lunge::Shutdown( Hourglass::Entity * entity )
{
	hg::Motor* motor = entity->GetComponent<hg::Motor>();
	motor->SetEnabled( false );
	motor->SetSpeed( motor->GetMaxSpeed() );
	entity->GetComponent<hg::Separation>()->SetEnabled( true );
	
}

Hourglass::IBehavior::Result AI_Lunge::Update( Hourglass::Entity* entity )
{
	if (IsAtDestination())
	{
		if (m_RecoverTimer == 0.0f)
		{
			hg::Motor* motor = entity->GetComponent<hg::Motor>();
			motor->SetEnabled( false );
		}

		Recover();

		if (IsFullyRecovered())
		{
			return kSUCCESS;
		}		
	}	
	
	m_Traveled += m_Speed * hg::g_Time.Delta();

	const Vector3& shadowDir = -entity->GetTransform()->Forward();
	PosAndColorShadowCopies( shadowDir );

	return kRUNNING;
}

Hourglass::IBehavior* AI_Lunge::MakeCopy() const
{
	AI_Lunge* copy = (AI_Lunge*)IBehavior::Create( SID(AI_Lunge) );
	copy->m_Dist = m_Dist;
	copy->m_Anim = m_Anim;
	copy->m_Speed = m_Speed;
	copy->m_ShieldColor = m_ShieldColor;
	copy->m_ShadowColor = m_ShadowColor;
	copy->m_NumShadows = m_NumShadows;
	copy->m_ShadowLifetime = m_ShadowLifetime;
	copy->m_RecoverLength = m_RecoverLength;
	copy->m_Inverted = m_Inverted;

	return copy;
}

void AI_Lunge::Recover()
{	
	m_RecoverTimer += hg::g_Time.Delta();
}

bool AI_Lunge::IsFullyRecovered()
{
	return m_RecoverTimer >= m_RecoverLength;
}

bool AI_Lunge::IsAtDestination()
{
	return m_Traveled >= m_Dist;
}

void AI_Lunge::PosAndColorShadowCopies(const Vector3& shadowDir)
{
	float divNumShadows = 1.0f / m_NumShadows;
	float shadowLifeTimeInv = 1.0f / m_ShadowLifetime;	

	for (unsigned int i = 0; i < m_NumShadows; ++i)
	{
		float shadowOffset = (i * divNumShadows) * m_Dist;
		float ageOfShadow = (m_Traveled - shadowOffset) / m_Speed;
		if (ageOfShadow >= 0.0f && ageOfShadow <= m_ShadowLifetime)
		{
			float colorRatio = ageOfShadow * shadowLifeTimeInv;
			Color shadowColor = Color::Lerp( m_ShieldColor, m_ShadowColor, colorRatio );
			Vector3 pos = m_LungeStart + shadowDir * shadowOffset;
			hg::DebugRenderer::DrawSphere( pos, m_ShadowRadius, shadowColor, m_ShadowSegments );
		}
	}
}
