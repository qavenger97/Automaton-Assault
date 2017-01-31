#include "GamePch.h"

#include "BhvAIAttack.h"
#include "Projectile.h"

extern bool g_bDebugAIBehaviors;

void BhvAIAttack::LoadFromXML(tinyxml2::XMLElement* data)
{
	const char* targetTag = data->Attribute("target_tag");

	if (targetTag)
		m_TargetTag = WSID(targetTag);

	m_AttackInterval = 1.0f;
	data->QueryFloatAttribute("attack_interval", &m_AttackInterval);

	m_AttackDistance = 1.0f;
	data->QueryFloatAttribute("attack_dist", &m_AttackDistance);

	const char* atkAnim = data->Attribute( "atk_anim_1" );
	m_AttackAnims[0] = WSID(atkAnim);

	atkAnim = data->Attribute( "atk_anim_1" );
	m_AttackAnims[1] = WSID(atkAnim);	

	atkAnim = data->Attribute( "atk_anim_2" );
	m_AttackAnims[2] = WSID( atkAnim );

	data->QueryFloatAttribute( "atk_anim_0_speed", &m_AttackAnimSpeeds[0] );
	data->QueryFloatAttribute( "atk_anim_1_speed", &m_AttackAnimSpeeds[1] );
	data->QueryFloatAttribute( "atk_anim_2_speed", &m_AttackAnimSpeeds[2] );

	m_Animation = nullptr;
}

void BhvAIAttack::Init(Hourglass::Entity* entity)
{
	if (m_Target == nullptr)
	{
		m_Target = Hourglass::Entity::FindByTag( m_TargetTag );
	}

	m_NextAttackTime = hg::g_Time.GameElapsed();
	m_DamageDealt = false;

	if (m_Animation == nullptr)
	{
		m_Animation = entity->GetComponent<hg::Animation>();
	}

	if (m_Motor == nullptr)
	{
		m_Motor = entity->GetComponent<hg::Motor>();
	}
}

Hourglass::IBehavior::Result BhvAIAttack::Update(Hourglass::Entity* entity)
{
	if (g_bDebugAIBehaviors)
	{
		hg::DevTextRenderer::DrawText_WorldSpace("BhvAIAttack", entity->GetPosition());
	}

	if (!m_Target || !entity)
		return IBehavior::kFAILURE;

	XMVECTOR vToTarget = m_Target->GetTransform()->GetWorldPosition() - entity->GetTransform()->GetWorldPosition();
	vToTarget = XMVectorSetY(vToTarget, 0.0f);
	float lenSq;
	XMStoreFloat(&lenSq, XMVector3LengthSq(vToTarget));

	// Target is out of range, we are done attacking
	if (lenSq > m_AttackDistance * m_AttackDistance && hg::g_Time.GameElapsed() > m_AttackStartTime + m_AttackInterval)
		return IBehavior::kSUCCESS;

	if (hg::g_Time.GameElapsed() >= m_NextAttackTime)
	{
		m_AttackStartTime = hg::g_Time.GameElapsed();
		int atkId = Math::RNG::Range( 0, kNumAttacks - 1 );
		StrID atkClip = m_AttackAnims[atkId];
		m_Animation->Play( atkClip, m_AttackAnimSpeeds[atkId] );
		m_NextAttackTime = hg::g_Time.GameElapsed() + m_AttackInterval;
		m_DamageDealt = false;
	}

	if (hg::g_Time.GameElapsed() >= m_NextAttackTime - 1.6f && !m_DamageDealt)
	{
		DamageMessage dmg(kDmgType_ChargerBotMelee);
		m_Target->SendMsg(&dmg);
		m_DamageDealt = true;
	}

	m_Motor->SetDesiredForward( XMVector3Normalize( vToTarget ) );

	return IBehavior::kRUNNING;
}

Hourglass::IBehavior* BhvAIAttack::MakeCopy() const
{
	BhvAIAttack* copy = (BhvAIAttack*)IBehavior::Create(SID(BhvAIAttack));

	copy->m_TargetTag = m_TargetTag;
	copy->m_AttackInterval = m_AttackInterval;
	copy->m_AttackDistance = m_AttackDistance;

	for (unsigned int i = 0; i < kNumAttacks; ++i)
	{
		copy->m_AttackAnims[i] = m_AttackAnims[i];
		copy->m_AttackAnimSpeeds[i] = m_AttackAnimSpeeds[i];
	}

	return copy;
}
