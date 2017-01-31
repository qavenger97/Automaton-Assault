#pragma once

#define MAX_NUM_GUN 3
class Health;
namespace Hourglass
{
	class Entity;
	class Transform;
}
class PlayerComp : public Hourglass::IComponent
{
public:

	virtual int GetTypeID() const { return s_TypeID; }

	void LoadFromXML( tinyxml2::XMLElement* data );

	virtual void Init();
	virtual void Start();
	virtual void Update();
	void OnMessage(hg::Message* msg);

	const Vector3& GetPosMouseWorld() const { return m_PosMouseWorld; }
	void SetMovementLock( bool movementLock ) { m_MovementLock = movementLock; }
	void SetSpeed( float speed ) { m_Speed = speed; }
	void SetGun(hg::Entity* gun, int index);
	void PostInit();
	float GetCurrentAmmoRatio()const;
	float GetCurrentHealthRatio()const;
	float GetCurrentTotalAmmoRatio()const;
	void HandleControl();
	void Dodge(const Vector3 moveDir);
	void EndDodge();
	bool IsDodging() const;
	//float GetGroundOffset() const;
	bool RestoreHP(float value);
	bool RestoreAmmo(float value);
	// Check if player is alive
	bool IsPlayerAlive() const;

	hg::Entity* GetGunLaser();

	/**
	* Make a copy of this component
	*/
	virtual hg::IComponent* MakeCopyDerived() const;

	static uint32_t s_TypeID;
private:

	void SwitchToWeapon(int slot);

	void UpdateGunTransform();

private:

	enum Dir
	{
		kUp,
		kDown,
		kLeft,
		kRight
	};

	struct Anims
	{
		// full body
		StrID Default;
		StrID Fire;
		StrID Dodge;

		// lower
		StrID Fwd;
		StrID FwdLeft;
		StrID FwdRight;
		StrID Bwd;
		StrID BwdLeft;
		StrID BwdRight;
		StrID Right;
		StrID Left;
		StrID Idle;
	} m_Anims;

	int m_curGun;
	hg::Entity* m_gun[MAX_NUM_GUN];
	hg::Transform* upper;
	hg::Transform* lower;
	hg::Transform* upperBone;
	hg::Transform* dodgeBody;
	hg::Animation* animation = nullptr;
	float gunHeight;
	Quaternion upperRotOffset;

	uint32_t m_MovementLock : 1;
	uint32_t m_Dodging : 1;
	Vector3 m_PosMouseWorld;
	Vector3 m_MoveDirection;
	void(*Integrate)(XMVECTOR&, XMVECTOR&, float, const XMVECTOR&);
	
	Health* m_health;
	float m_rollTimer;
	float m_Speed;
	float m_RunSpeed;
	float m_DodgeTime;
	float m_DodgeDuration;
	float m_DodgeSpeed;
	bool m_debug;
	bool m_freeAim;
	bool m_needRotate;

	float m_DamageOverlay;
	float m_DamageDecayTimer;
	UINT m_powerUp;
	int m_powerUpAmount;
};