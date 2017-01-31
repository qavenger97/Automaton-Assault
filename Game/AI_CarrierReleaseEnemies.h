#pragma once

class AI_CarrierReleaseEnemies : public hg::IAction
{
public:
	void LoadFromXML( tinyxml2::XMLElement* data );

	void Init( hg::Entity* entity ) override;

	hg::IBehavior::Result Update( hg::Entity* entity ) override;

	hg::IBehavior* MakeCopy() const;

private:

	float m_Timer;
	float m_Duration;
	uint32_t m_CurrRow;

	static const int kRows = 3;
	static const int kCols = 4;
	hg::Entity* m_Enemies[kRows * kCols];
	hg::Transform* m_Carrier;

};