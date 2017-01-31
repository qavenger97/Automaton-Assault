#pragma once
#include "../Engine/Core/Message.h"
namespace GameMessageType
{
	enum
	{
		kDamage = hg::MessageType::kEngineMessageCount,
		kDeath,
		kFire,
		kPlayerDeath,
		kPowerUp,
		kWin
	};
}

enum DamageType
{
	kDmgType_Generic,
	kDmgType_ChargerBotMelee,
	kDmgType_Bullet,
	kDmgType_BossHeadButt
};

enum PowerUpType
{
	kNoPowerUp				= 0b00000000,
	kPowerUp_Ricochet		= 0b00000001,
	kPowerUp_Shield			= 0b00000010,
	kPowerUp_Boss			= 0b01000000,
	kPowerUp_End			= 0b10000000,
};

class FireMessage : public hg::Message
{
	int GetType()const { return GameMessageType::kFire; }
};

class PowerUpMessage : public hg::Message
{
	UINT m_type;
	float m_value;
public:
	PowerUpMessage(UINT type, float value) : m_type(type), m_value(value){}
	int GetType()const { return GameMessageType::kPowerUp; }
	float GetAmount()const { return m_value; }
	UINT GetPowerUpType()const { return m_type; }
};

class DamageMessage : public hg::Message
{
	DamageType m_DamageType;
public:
	DamageMessage() : m_DamageType(kDmgType_Generic) {}
	DamageMessage(DamageType _type) : m_DamageType(_type) {}

	int GetType() const { return GameMessageType::kDamage; }

	// TODO: add damage amount
	void SetDamageType(DamageType _type) { m_DamageType = _type; }
	DamageType GetDamageType() const { return m_DamageType; }
};

class DeathMessage : public hg::Message
{
public:
	int GetType() const { return GameMessageType::kDeath; }
	// TODO: add damage amount
	hg::Entity* sender;
};

class PlayerDeathMessage : public hg::Message
{
public:
	int GetType() const { return GameMessageType::kPlayerDeath; }
	// TODO: add damage amount
};

class LevelCompleteMessage : public hg::Message
{
public:
	int GetType() const { return GameMessageType::kWin; }
};