#pragma once

#include "Common.h"

enum TurretLightState
{
	kTurretLight_Red,
	kTurretLight_Yellow,
	kTurretLight_Off,

	kTurretLightStateCount
};

static const uint16_t kTurretVisionMask = COLLISION_DEFAULT_MASK | COLLISION_UNWALKABLE_MASK | COLLISION_PLAYER_MASK;

class GunLogic;

class Turret : public hg::IComponent
{
	DECLARE_COMPONENT_TYPEID
public:
	hg::IComponent* MakeCopyDerived() const;

	void Start();

	void Update();

	void OnMessage(hg::Message* msg);

private:
	bool CanSeePlayer(hg::Entity* player) const;

	hg::Transform* GetGunTransform() const;

	hg::Entity*			m_Player;
	hg::Transform*		m_Gun;
	hg::MeshRenderer*	m_LightMesh;
	hg::Material*		m_LightMaterials[kTurretLightStateCount];
};