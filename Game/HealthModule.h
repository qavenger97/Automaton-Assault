#pragma once

namespace Hourglass
{
	class PrimitiveRenderer;
}
class Health : public hg::IComponent
{
public:
	virtual int GetTypeID() const { return s_TypeID; }

	void LoadFromXML(tinyxml2::XMLElement* data);

	float GetValue() const { return m_Value; }
	void SetValue(float val) { m_Value = val; }

	void Init();
	void Update();
	bool RestoreHealth(float value);
	void SetInvulnerable( bool invulnerable ) { m_Invulnerable = invulnerable; }

	void OnMessage(hg::Message* msg);

	void DealDamage( float dmgVal );

	hg::IComponent* MakeCopyDerived() const;

	float GetHealthRatio()const;

	static uint32_t s_TypeID;
private:
	float	m_Value;
	float	m_maxValue;
	bool	m_PlayingFlashEffect;
	float	m_FlashEffectEndTime;
	Color	m_MeshColor;			// For flashing effect
	hg::RenderComponent* m_Renderer;
	char m_Invulnerable : 1;
	char m_InDirect : 1;

};
