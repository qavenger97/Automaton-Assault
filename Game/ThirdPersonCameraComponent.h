#pragma once

class ThirdPersonCameraComponent : public hg::IComponent
{
public:
	ThirdPersonCameraComponent();
	void LoadFromXML(tinyxml2::XMLElement * data);
	virtual int GetTypeID() const { return s_TypeID; }
	
	void Init();
	void Update();
	void AttachTo(hg::Entity* target);
	virtual hg::IComponent* MakeCopyDerived() const;
	static uint32_t s_TypeID;
	void SetOffset(const float x, const float y, const float z);
	void SetDistance(const float distance);
	void SetSpeed(const float speed);
	void SetDesireOffsetDistance(FXMVECTOR& offset);
	void OnMessage(hg::Message* msg)override;
	float GetDistance()const { return m_distance; }
	void RecenterCamera();
private:
	hg::Entity* m_attached;
	XMFLOAT3 m_direction;
	XMFLOAT3 m_curOffsetDistance;
	XMFLOAT3 m_desireOffsetDistance;
	float m_distance;
	float m_maxDistance;
	float m_speed;
};