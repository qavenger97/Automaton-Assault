#pragma once

#include "Turret.h"

class TurretSkinnedMesh : public hg::IComponent
{
	DECLARE_COMPONENT_TYPEID
public:
	TurretSkinnedMesh();

	hg::IComponent* MakeCopyDerived() const;

	void Start();

	void Update();

	void OnMessage(hg::Message* msg);

private:
	hg::Transform* GetMuzzle() const;

	bool CanSeePlayer(hg::Entity* player) const;

	bool				m_bInitTransform;

	hg::Entity*			m_Player;
	hg::Transform*		m_Gun;
	hg::Animation*		m_Animation;

	hg::Transform		m_Muzzle;
	float				m_Yaw;
	XMVECTOR			m_GunForward;
	XMVECTOR			m_GunRight;
};