#include "GamePch.h"
#include "AI_Tunnel.h"
#include "HealthModule.h"
#include "GameMessage.h"

void AI_Tunnel::LoadFromXML( tinyxml2::XMLElement* data )
{
	m_MovingEntityName = WSID("0");
	m_MovingEntity = nullptr;
	const char* movingEntityName = data->Attribute("entity_name");
	if (movingEntityName)
	{
		m_MovingEntityName = WSID(movingEntityName);
	}

	m_DiveLength = 1.0f;
	data->QueryFloatAttribute( "dive_length", &m_DiveLength );

	m_SurfaceInterval = 4.0f;
	data->QueryFloatAttribute( "surface_interval", &m_SurfaceInterval );

	m_NumDives = 17;
	data->QueryUnsignedAttribute( "num_dives", &m_NumDives );

	m_ArcPrediction = 1.7f;
	data->QueryFloatAttribute( "arc_prediction", &m_ArcPrediction );

	m_PushRadius = 3.0f;
	data->QueryFloatAttribute( "push_radius", &m_PushRadius );
	
	m_PushInterval = 0.1f;
	data->QueryFloatAttribute( "push_interval", &m_PushInterval );

	m_TargetOffsetRadius = 2.0f;
	data->QueryFloatAttribute( "target_offset", &m_PushInterval );

	m_DiveCount = 0;
	data->QueryUnsignedAttribute( "dive_count", &m_DiveCount );
}

void AI_Tunnel::Start()
{
	m_DiveTime = 0.0f;
	m_DiveCount = 0;
	m_PushBegin = Vector3::Zero;
	m_PushEnd = Vector3::Zero;
}

void AI_Tunnel::Init( Hourglass::Entity* entity )
{
	m_DiveTime = 0.0f;
	m_DiveCount = 0;
	m_Player = hg::Entity::FindByTag( SID(Player) );
	if (m_MovingEntityName == SID(0))
		m_MovingEntity = entity;
	else
		m_MovingEntity = hg::Entity::FindByName( m_MovingEntityName );
	TargetStrikingPosition(m_MovingEntity);	
}

hg::IBehavior::Result AI_Tunnel::Update( Hourglass::Entity* entity )
{
	m_DiveTime += hg::g_Time.Delta();

	// Base our travel times are based off of where we are in terms of dive count
	float f = m_DiveTime;
	unsigned int currOffset = m_DiveCount % submergedInterval;
	float travelLength = ( currOffset == submergedOffset) ? m_SurfaceInterval : m_DiveLength;

	f /= travelLength;

	if (m_DiveTime >= travelLength)
		m_DiveTime = travelLength;

	// Travel
	Vector3 direction = Math::Slerp( m_StartNl, m_EndNl, f );
	float length = Math::Lerp( m_StartL, m_EndL, f );
	m_MovingEntity->GetTransform()->SetPosition( m_Pivot + direction * length );

	PushBackPlayer( m_MovingEntity );

	// Set new travel targets if needed, exit if complete
	if (m_DiveTime == travelLength)
	{
		m_DiveTime = 0;

		if (++m_DiveCount == m_NumDives)
			return kSUCCESS;

		if (m_DiveCount & 1)
			TargetPlayerPosition( m_MovingEntity );
		else
			TargetStrikingPosition( m_MovingEntity );

		if (m_DiveCount % 4 == 1)
		{
			// Player camera shake and sound effect when boss is coming out from underground
			hg::g_AudioSystem.PostEvent(AK::EVENTS::PLAY_BOSS_LEAVEGROUND, entity->GetPosition());
			hg::Entity::Assemble(SID(FX_Ground000), XMVectorSetY(m_Player->GetPosition(), 0.5f));
			hg::Entity::Assemble(SID(FX_BossCameraShake), m_Player->GetPosition());
		}
		else if (m_DiveCount % 4 == 0)
		{
			hg::g_AudioSystem.PostEvent(AK::EVENTS::PLAY_BOSS_ENTERGROUND, entity->GetPosition());
			hg::Entity::Assemble(SID(FX_Ground000), XMVectorSetY(m_MovingEntity->GetPosition(), 0.5f));
		}
	}

	return kRUNNING;
}

void AI_Tunnel::TargetStrikingPosition( hg::Entity* entity )
{
	Vector3 strikingPos = m_MovingEntity->GetTransform()->Forward() * m_ArcPrediction;
	strikingPos += m_MovingEntity->GetPosition();

	// Striking position above/below ground determined by dive count
	strikingPos.y = (m_DiveCount % 4 == 0) ? -64.0f : 32.0f;

	m_Pivot = m_MovingEntity->GetPosition() + strikingPos;
	m_Pivot *= 0.5f;
	m_Pivot.y = 0.0f;

	Vector3 end = strikingPos - m_Pivot;
	Vector3 start = m_MovingEntity->GetPosition() - m_Pivot;

	m_StartNl = start;
	m_StartNl.Normalize();
	m_EndNl = end;
	m_EndNl.Normalize();
	m_StartL = start.Length();
	m_EndL = end.Length();
}

void AI_Tunnel::TargetPlayerPosition( hg::Entity* entity )
{
	m_Pivot = m_MovingEntity->GetPosition() + m_Player->GetPosition();
	m_Pivot *= 0.5f;
	m_Pivot.y = 0.0f;
	Vector3 start = m_MovingEntity->GetPosition() - m_Pivot;
	Vector3 randomV = Math::RNG::RandomOnSphere();
	Vector3 fwd = m_Player->GetTransform()->Forward();
	float f = 0.3f + Math::RNG::Range( 0.0f, 0.7f );
	Vector3 offset = Math::Slerp( randomV, fwd, f );
	Vector3 end = m_Player->GetPosition() + offset * m_TargetOffsetRadius * Math::RNG::Range(0.0f, 1.0f);
	end.y = 0;
	end -= m_Pivot;

	end.y = 0.0f;
	m_StartNl = start;
	m_StartNl.Normalize();
	m_EndNl = end;
	m_EndNl.Normalize();
	m_StartL = start.Length();
	m_EndL = end.Length();
}

void AI_Tunnel::PushBackPlayer( hg::Entity* entity )
{
	if (m_PushTime > 0.0f)
	{
		m_PushTime -= hg::g_Time.Delta();
		float f = 1.0f - m_PushTime / m_PushInterval;
		Vector3 newPos = Vector3::Lerp( m_PushBegin, m_PushEnd, f );
		m_Player->GetTransform()->SetPosition( newPos );
		return;
	}
	
	Vector3 playerPos = m_Player->GetPosition();
	Vector3 entityPos = m_MovingEntity->GetPosition();
	Vector3 toVector = playerPos - entityPos;
	float lenSq = toVector.LengthSquared();

	float radiusSq = m_PushRadius * m_PushRadius;
	if (lenSq <= radiusSq)
	{
		Vector3 toVectorG = toVector;
		toVectorG.y = 0;

		if (toVectorG.Length() == 0.0f)
			toVectorG = Vector3( float( rand() ), 0.0f, float( rand() ) );
		
		toVectorG.Normalize();

		float distG = fabsf(toVector.y);
		float moveDist = m_PushRadius;

		toVectorG *= moveDist;
		Vector3 endPos = entityPos + toVectorG;
		endPos.y = playerPos.y;

		m_PushBegin = playerPos;
		m_PushEnd = endPos;
		m_PushTime = m_PushInterval;
		
		if (m_Player->GetComponent<Health>())
		{
			hg::Entity* particle = hg::Entity::Assemble( SID( FX_BulletImpact_Metal ) );
			particle->GetTransform()->SetPosition( playerPos );
			DamageMessage dmg( kDmgType_BossHeadButt );
			m_Player->SendMsg( &dmg );
		}
	}
}

hg::IBehavior* AI_Tunnel::MakeCopy() const
{
	AI_Tunnel* copy = (AI_Tunnel*)IBehavior::Create( SID( AI_Tunnel ) );
	copy->m_DiveLength = m_DiveLength;
	copy->m_SurfaceInterval = m_SurfaceInterval;
	copy->m_NumDives = m_NumDives;
	copy->m_PushRadius = m_PushRadius;
	copy->m_PushInterval = m_PushInterval;
	copy->m_TargetOffsetRadius = m_TargetOffsetRadius;
	copy->m_DiveTime = 0.0f;
	copy->m_Pivot = Vector3::Zero;
	copy->m_MovingEntityName = m_MovingEntityName;
	return copy;
}