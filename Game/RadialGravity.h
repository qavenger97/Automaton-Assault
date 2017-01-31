#pragma once

class RadialGravity : public hg::IComponent
{
public:
	virtual int GetTypeID() const { return s_TypeID; }
	static uint32_t s_TypeID;

	void LoadFromXML( tinyxml2::XMLElement* data );

	virtual void Start();

	void ActivateBridgeExit() { m_BridgeExitMode = true; }

	void DoomFall();

	void Update();

	void OnEvent(const StrID& eventName);

	hg::IComponent* MakeCopyDerived() const;

private:

	float m_DropRadiusSq;
	float m_OuterRingRadiusSq;
	float m_GroundY;
	float m_DoomTimer;

	float m_HitBottomTime;
	float m_DoomTime;

	float m_BridgeLeftBounds;
	float m_BridgeRightBounds;
	float m_BridgeBottomBounds;
	float m_BridgeTopBounds;

	uint32_t m_BridgeExitMode : 1;
	uint32_t m_IsDoomFalling : 1;
	uint32_t m_BridgeIsRetracted : 1;

	XMVECTOR m_FakePlayerVelocity;

	hg::Transform* m_Player;
	hg::DynamicCollider* m_PlayerDC;
};