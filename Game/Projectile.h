#pragma once

#include "GameMessage.h"

class Projectile : public hg::IComponent
{
public:
	Projectile();
	void LoadFromXML(tinyxml2::XMLElement* data);
	void Init();
	void Start();
	void SetSpeed(const float speed);
	void SetLifeTime(const float life);
	void AddDamage(float damage);
	void Update();
	void SetBounce(bool enable);
	virtual int GetTypeID() const { return s_TypeID; }
	/**
	* Make a copy of this component
	*/
	virtual IComponent* MakeCopyDerived() const override;
public:
	static uint32_t s_TypeID;
private:
	float m_speed;
	float m_lifeTime;
	float m_timer;
	float m_damage;
	bool  m_Bounce;
	float m_Gravity;
	Vector3	m_Velocity;
	bool  m_live;
};

