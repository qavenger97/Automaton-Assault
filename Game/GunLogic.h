#pragma once

namespace Hourglass
{
	class Entity;
	class Transform;
	class Animation;
}
class GunLogic : public hg::IComponent
{
public:
	enum State
	{
		IDLE = 0,
		FIREING,
		CHARGING,
		RELOADING,
		RELOADED,
		NUMOFSTATE
	};

	enum GunType
	{
		AUTO = 0,
		SEMI,
		SINGLE,
		NUM_OF_TYPE
	};

public:
	static void ConfigureMachineGun(GunLogic* gun);
	static void ConfigureShotGun(GunLogic* gun);
	static void ConfigureTurretGun(GunLogic* gun);
	GunLogic();
	//void LoadFromXML(tinyxml2::XMLElement* data);
	void Init();
	unsigned int GetGunState() { return m_state; }
	void SetMaxAmmo(int maxAmmo);
	void SetMaxAmmoPerMag(int maxAmmoPerMag);
	void SetReloadTime(float reloadTime);
	void SetRpm(float rpm);
	void SetSpeard(float maxSpeardRad, float fullySpeardRatio);
	void SetSpeardDegree(float maxSpeardAngle, float fullySpeardRatio);
	void SetAssembledBulletName(StrID bullet);
	void SetDamage(float damage);
	void SetType(GunType type);
	void SetSpawnPoint(hg::Transform* position);
	void SetGunFireAudioEventId(AudioEvent eventId);
	void Update();
	void FireBullet();
	void FireBullets();
	void Fire();
	void Firing();
	void SetBounce(bool enable);
	void Cease();
	void Reload();
	void Reloading();
	bool IsReloading();
	void OnGunSwitching();
	void SwitchState(State nextState);

	void SetBulletPerShot(int amount);
	bool AddAmmo(int amount);
	float GetAmmoRatio()const;
	float GetTotalAmmoRatio()const;
	virtual int GetTypeID() const { return s_TypeID; }
	/**
	* Make a copy of this component
	*/
	virtual IComponent* MakeCopyDerived() const override;
private:

	void EmitGunFireSound() const;

	void EmitReloadSound() const;

public:
	static uint32_t s_TypeID;

private:
	GunType m_type;
	State m_state;
	StrID m_BulletName;
	hg::Transform* m_spawnPos;
	hg::Animation* m_playerAnim;
	AudioEvent m_GunFireAudioEventId;
	hg::Entity* m_holder;
	float m_RPM = 0;
	float m_Speard = 0;
	float m_damage = 0;
	float m_reloadTime = 0;
	float m_inv_reloadTime = 0;

	float m_fullySpeardRatio = 0;
	int m_bulletsPerShot = 0;

	int m_maxAmmoPerMag = 0;
	int m_maxAmmo = 0 ;

	float m_fireRate = 0;

	int m_curAmmoInMag = 0;
	int m_curAmmoTotal = 0;

	float m_inv_MaxAmmoPerMag = 0;

	float m_timer = 0;
	float m_fireTimer = 0;
	float m_lightStep = 0;
	float m_speardRange = 0;
	float m_speardStep = 0;
	bool m_bounce = false;
	hg::Light*	m_MuzzleLight;
	float		m_MuzzleLightTimer;
	float		m_MuzzleLightRadius;
};

