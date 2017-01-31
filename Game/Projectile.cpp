
#include "GamePch.h"
#include "HealthModule.h"
#include "Projectile.h"
#include "Common.h"
#include "../Engine/Component/ParticleEmitter.h"

uint32_t Projectile::s_TypeID = hg::ComponentFactory::GetGameComponentID();
Projectile::Projectile()
	:
	m_damage(0),
	m_live(true)
{
}
void Projectile::LoadFromXML(tinyxml2::XMLElement * data)
{
	data->QueryFloatAttribute("speed", &m_speed);
	data->QueryFloatAttribute("life", &m_lifeTime);
	data->QueryFloatAttribute("damage", &m_damage);

	m_Bounce = false;
	data->QueryBoolAttribute("bounce", &m_Bounce);

	m_Gravity = 0.0f;
	data->QueryFloatAttribute("gravity", &m_Gravity);
}

void Projectile::Init()
{
}

void Projectile::Start()
{
	m_timer = 0;
	m_Velocity = GetEntity()->GetTransform()->Forward() * m_speed;
}

void Projectile::SetSpeed(const float speed)
{
	m_speed = speed;
	m_Velocity = GetEntity()->GetTransform()->Forward() * speed;
}

void Projectile::SetLifeTime(const float life)
{
	m_lifeTime = life;
}

void Projectile::AddDamage(float damage)
{
	m_damage += damage;
}

void Projectile::Update()
{
	if (m_timer >= m_lifeTime)
	{
		hg::Light* light = GetEntity()->GetComponent<hg::Light>();
		if(light)
			light->SetEnabled(false);
		m_live = false;
	}
	if (m_live)
	{
		m_timer += hg::g_Time.Delta();
		//assert(GetEntity()->GetComponent<hg::PrimitiveRenderer>() == nullptr);
		//assert(GetEntity()->GetComponent<hg::MeshRenderer>() == nullptr);


		SimpleMath::Vector3 pos = GetEntity()->GetTransform()->GetWorldPosition();
		SimpleMath::Vector3 forward = GetEntity()->GetTransform()->Forward();
		//SimpleMath::Vector3 vel = (forward * m_speed + m_VelocityY) * hg::g_Time.Delta();
		SimpleMath::Vector3 end = pos + m_Velocity * hg::g_Time.Delta();

		if (m_Gravity != 0.0f)
			m_Velocity += Vector3(0, -10, 0) * m_Gravity * hg::g_Time.Delta();

		//hg::DebugRenderer::DrawLine(pos, pos + forward * 1.5f);
		hg::Ray ray(pos, end);
		hg::Entity* hit;
		GetEntity()->GetTransform()->Translate(end - pos);
		Vector3 hitPos;
		Vector3 surfaceNormal;

		if (hg::g_Physics.RayCast(ray, &hit, &hitPos, &surfaceNormal, COLLISION_BULLET_HIT_MASK))
		{
			bool causeDamage = false;

			if (hit->GetComponent<Health>())
			{
				hg::Entity* particle = hg::Entity::Assemble(SID(FX_BulletImpact_Metal));
				particle->GetTransform()->SetPosition(hitPos);
				particle->GetTransform()->Rotate(GetEntity()->GetTransform()->GetWorldRotation());

				//OutputDebugStringA(hit->GetName().c_str());
				DamageMessage dmg(kDmgType_Bullet);
				hit->SendMsg(&dmg);
				causeDamage = true;
			}
			else
			{
				hg::Entity* particle = hg::Entity::Assemble(SID(FX_BulletImpact_Metal));
				particle->GetTransform()->SetPosition(hitPos);
				particle->GetTransform()->SetRotation(GetEntity()->GetTransform()->GetWorldRotation());
			}

			if (m_Bounce && !causeDamage)
			{
				// Bounce projectile against surface
				m_Velocity = Vector3::Reflect(m_Velocity, surfaceNormal);
				Vector3 dir = m_Velocity;
				dir.Normalize();
				GetEntity()->GetTransform()->SetPosition(hitPos + dir * 0.01f);
			}
			else
			{
				hg::Light* light = GetEntity()->GetComponent<hg::Light>();
				if (light)
					light->SetEnabled(false);
				m_live = false;
			}
		}

		Vector3 dir = m_Velocity;
		dir.Normalize();
		GetEntity()->GetTransform()->LookAt(GetEntity()->GetPosition() + dir);
	}

	if (!m_live)
	{
		GetEntity()->Destroy();
		return;
	}
}

void Projectile::SetBounce(bool enable)
{
	m_Bounce = enable;
}

hg::IComponent * Projectile::MakeCopyDerived() const
{
	Projectile* cpy = (Projectile*)IComponent::Create(SID(Projectile));
	*cpy = *this;
	return cpy;
}
