#include "GamePch.h"

#include "AI_SpiderBotBehavior.h"
#include "Projectile.h"
#include "Renderer\DebugRenderer.h"
#include "GameUtils.h"

extern bool g_bDebugAIBehaviors;

enum SpiderBotState
{
	kSpiderBot_Chase,
	kSpiderBot_Jump,
	kSpiderBot_Attack,
};

const float kSpiderBotJumpInverval = 2.0f;
const float kSpiderBotJumpTime = 0.75f;
const float kSpiderBotJumpMaxHeight = 2.0f;

const float kSpiderBotJumpMinDist = 6.0f;
const float kSpiderBotJumpMaxDist = 8.0f;
const float kSpiderBotJumpMinDistSqr = kSpiderBotJumpMinDist * kSpiderBotJumpMinDist;
const float kSpiderBotJumpMaxDistSqr = kSpiderBotJumpMaxDist * kSpiderBotJumpMaxDist;

const float kSpiderBotStopDist = 3.0f;
const float kSpiderBotStopDistSqr = kSpiderBotStopDist * kSpiderBotStopDist;

const float kSpiderBotAttackInterval = 2.6f;
const float kSpiderBotAttackTime = 1.0f;

const float kSpiderBotClickSoundInterval = 5.5f;

void AI_SpiderBotBehavior::LoadFromXML(tinyxml2::XMLElement* data)
{

}

void AI_SpiderBotBehavior::Init(hg::Entity* entity)
{
	m_Target = nullptr;
	m_State = kSpiderBot_Chase;
	m_Animation = entity->GetComponent<hg::Animation>();
	m_VelocityY = 0.0f;
	/*m_Anims.idle = SID(idle);
	m_Anims.walk = SID(walk);
	m_Anims.jump = SID(jump);
	m_Anims.bigJump = SID(bigJump);*/
}


Hourglass::IBehavior::Result AI_SpiderBotBehavior::Update(hg::Entity* entity)
{
	if (g_bDebugAIBehaviors)
	{
		static const char* stateName[] =
		{
			"AI_SpiderBotBehavior: Chase",
			"AI_SpiderBotBehavior: Jump",
			"AI_SpiderBotBehavior: Attack",
		};

		hg::DevTextRenderer::DrawText_WorldSpace(stateName[m_State], entity->GetPosition());
	}

	// Get player as target if we don't have a target
	if (!m_Target)
	{
		m_Target = hg::Entity::FindByTag(SID(Player));

		// Can't find player, stop behavior
		if (!m_Target)
			return IBehavior::kFAILURE;
	}

	// Play deploy sound when spawn
	if (!m_Deployed)
	{
		hg::g_AudioSystem.PostEvent(AK::EVENTS::PLAY_SPIDERBOT_DEPLOY, entity->GetPosition());
		m_Deployed = true;
	}

	// Play spider bot click sound
	m_ClickSoundTimer -= hg::g_Time.Delta();
	if (m_ClickSoundTimer <= 0)
	{
		hg::g_AudioSystem.PostEvent(AK::EVENTS::PLAY_SPIDERBOT_RAPIDCLICK, entity->GetPosition());
		m_ClickSoundTimer = kSpiderBotClickSoundInterval;
	}

	hg::Motor* motor = entity->GetComponent<hg::Motor>();
	if (motor) motor->SetMoveEnabled(true);

	if (m_State == kSpiderBot_Chase)
	{
		if (SpiderAttack_CheckCondition(entity))
		{
			m_State = kSpiderBot_Attack;
			Attack(entity);
		}
		else if (SpiderJump_CheckCondition(entity))
		{
			m_State = kSpiderBot_Jump;
			m_JumpStartPoint = entity->GetPosition();
			m_JumpEndPoint = m_Target->GetPosition();
		}
	}

	//animation = entity->GetComponent<hg::Animation>();
	switch (m_State)
	{
	case kSpiderBot_Chase:
		//animation->Play(m_Anims.walk, 1);
		SpiderChase_Update(entity);
		break;
	case kSpiderBot_Jump:
		//animation->Play(m_Anims.bigJump, 1);
		SpiderJump_Update(entity);
		break;
	case kSpiderBot_Attack:
		//animation->Play(m_Anims.walk, 1);
		SpiderAttack_Update(entity);
		break;
	}

	// Apply gravity to spider bot
	hg::DynamicCollider* dynCol = entity->GetComponent<hg::DynamicCollider>();
	float dist = m_VelocityY * hg::g_Time.Delta();

	float offsetY = dynCol->GetLocalCylinderShape().GetBottomY();
	Vector3 start = entity->GetPosition();
	start.y += offsetY - 0.01f;
	Vector3 end = start + Vector3(0, -1, 0) * dist;

	hg::Ray ray(start, end);
	Vector3 hitPos;
	hg::Entity* hitEnt = nullptr;

	if (hg::g_Physics.RayCast(ray, &hitEnt, &hitPos))
	{
		entity->GetTransform()->SetPosition(hitPos + Vector3(0, offsetY + 0.01f, 0));
		m_VelocityY = 0.0f;
		
		//hg::DebugRenderer::DrawLine(hitPos, hitPos);
	}
	else
	{
		entity->GetTransform()->Translate(Vector3(0, -1, 0) * dist);
		m_VelocityY += 10.0f * hg::g_Time.Delta();
	}

	//char buf[1024];
	//sprintf_s(buf, "D: %f\nG: %f\nN: %s", dist, m_VelocityY, hitEnt ? hitEnt->GetName().c_str() : "");
	//hg::DevTextRenderer::DrawText_WorldSpace(buf, hitPos);

	//hg::DebugRenderer::DrawAxis(*entity->GetTransform());

	// When player is no longer visible, move to path finding state
	if (m_State == kSpiderBot_Chase)
		if (!GameUtils::AI_CanSeePlayer(entity, m_Target, Vector3(0, 0.5f, 0)))
			return IBehavior::kSUCCESS;

	return IBehavior::kRUNNING;
}

Hourglass::IBehavior* AI_SpiderBotBehavior::MakeCopy() const
{
	AI_SpiderBotBehavior* cpy = (AI_SpiderBotBehavior*)IBehavior::Create(SID(AI_SpiderBotBehavior));

	cpy->m_Deployed = false;
	cpy->m_Target = nullptr;
	cpy->m_JumpStartTime = 0.0f;
	cpy->m_JumpTimer = 0.0f;
	cpy->m_AttackStartTime = hg::g_Time.GameElapsed() + 2.0f;		// First attack starts after spawn for 2 seconds
	cpy->m_AttackTimer = 0.0f;
	cpy->m_ClickSoundTimer = 3.5f;
	cpy->m_State = kSpiderBot_Chase;

	return cpy;
}

bool AI_SpiderBotBehavior::SpiderJump_CheckCondition(hg::Entity* entity) const
{
	if (!m_Target)
		return false;

	// Check jump distance. Fail if we're either too far away from or too close to player
	float distSqr = (m_Target->GetPosition() - entity->GetPosition()).LengthSquared();
	if (distSqr >= kSpiderBotJumpMaxDistSqr || distSqr <= kSpiderBotJumpMinDistSqr)
		return false;

	// Check jump timer
	float t = hg::g_Time.GameElapsed();
	if (t < m_JumpStartTime)
		return false;

	return true;
}

void AI_SpiderBotBehavior::SpiderJump_Update(hg::Entity* entity)
{
	hg::DynamicCollider* col = entity->GetComponent<hg::DynamicCollider>();
	if (!col)
		return;

	hg::Motor* motor = entity->GetComponent<hg::Motor>();
	if (!motor)
		return;

	if (!m_Animation->IsPlaying(SID(jump)))
		m_Animation->Play(SID(jump), 1.0f);

	motor->SetDesiredMove(Vector3::Zero);

	float t = hg::g_Time.GameElapsed();
	if (t >= m_JumpStartTime)
	{
		if (m_JumpTimer <= 0)	// Jump done
		{
			// Set up next jump timer	
			m_JumpStartTime = t + kSpiderBotJumpInverval;
			m_JumpTimer = kSpiderBotJumpTime;

			m_State = kSpiderBot_Chase;
			return;
		}

		// Calculate old position for moving vector
		float dt = min((kSpiderBotJumpTime - m_JumpTimer) / kSpiderBotJumpTime, 1.0f);
		Vector3 oldPos = Vector3::Lerp(m_JumpStartPoint, m_JumpEndPoint, dt);
		oldPos.y += sinf(dt * XM_PI) * kSpiderBotJumpMaxHeight;

		m_JumpTimer -= hg::g_Time.Delta();

		dt = min((kSpiderBotJumpTime - m_JumpTimer) / kSpiderBotJumpTime, 1.0f);
		Vector3 newPos = Vector3::Lerp(m_JumpStartPoint, m_JumpEndPoint, dt);
		newPos.y += sinf(dt * XM_PI) * kSpiderBotJumpMaxHeight;
		col->Move(newPos - oldPos);
	}
}

void AI_SpiderBotBehavior::SpiderChase_Update(hg::Entity* entity)
{
	if (!m_Target)
		return;

	hg::DynamicCollider* col = entity->GetComponent<hg::DynamicCollider>();
	if (!col)
		return;

	hg::Motor* motor = entity->GetComponent<hg::Motor>();
	if (!motor)
		return;


	Vector3 vecToTarget = m_Target->GetPosition() - entity->GetPosition();
	Vector3 plannarDir = vecToTarget;
	vecToTarget.y = 0.0f;
	plannarDir.y = 0.0f;

	plannarDir.Normalize();

	float sqrDist = vecToTarget.LengthSquared();
	vecToTarget.Normalize();
	motor->SetDesiredForward(vecToTarget);

	// Stop moving when close enough
	if (sqrDist < kSpiderBotStopDistSqr)
	{
		if (!m_Animation->IsPlaying(SID(idle)))
			m_Animation->Play(SID(idle), 1.0f);

		motor->SetDesiredMove(Vector3::Zero);
		return;
	}

	if (!m_Animation->IsPlaying(SID(walk)))
		m_Animation->Play(SID(walk), 2.0f);

	motor->SetDesiredMove(plannarDir);
}

bool AI_SpiderBotBehavior::SpiderAttack_CheckCondition(hg::Entity* entity) const
{
	if (!m_Target)
		return false;

	float t = hg::g_Time.GameElapsed();
	if (t < m_AttackStartTime)
		return false;

	return true;
}

void AI_SpiderBotBehavior::SpiderAttack_Update(hg::Entity* entity)
{
	if (!m_Animation->IsPlaying(SID(idle)))
		m_Animation->Play(SID(idle), 1.0f);

	m_AttackTimer -= hg::g_Time.Delta();

	if (m_AttackTimer <= 0)
		m_State = kSpiderBot_Chase;
}

void AI_SpiderBotBehavior::Attack(hg::Entity* owner)
{
	m_AttackTimer = kSpiderBotAttackTime;
	m_AttackStartTime = hg::g_Time.GameElapsed() + kSpiderBotAttackInterval;

	// Stop moving once start attack animation
	hg::Motor* motor = owner->GetComponent<hg::Motor>();
	motor->SetDesiredMove(Vector3::Zero);

	// Spawn projectile
	Vector3 forward = -owner->GetTransform()->Forward();
	Vector3 bulletPos = owner->GetPosition() + forward * 0.6f + Vector3(0, 0.25f, 0);
	Vector3 dirVec = m_Target->GetPosition() - owner->GetPosition();
	dirVec.y = 0.0f;
	XMVECTOR dir = hg::Transform::BuildLookAtRotationFromDirectionVector(dirVec);

	hg::Entity* spatter = hg::Entity::Assemble(SID(SpiderBotSpatter), bulletPos, dir);
	Projectile* bullet = spatter->GetComponent<Projectile>();

	bullet->AddDamage(10.0f);

	// Play spider bot spray sound
	hg::AudioSource* audioSource = owner->GetComponent<hg::AudioSource>();
	audioSource->PostAudioEvent(AK::EVENTS::PLAY_SPIDERBOT_SPRAY);
}
