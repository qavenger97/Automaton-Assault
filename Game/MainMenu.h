#pragma once
#include "../Engine/Component/IComponent.h"

class PlayerComp;

class MainMenu : public hg::IComponent
{
public:
	enum GameState {
		MAIN_MENU,
		LOAD_OPTION,
		LOAD_MENU,
		OPTION,
		LOADING,
		RUNNING,
		PAUSE,
		GAMEOVER,
		WIN,
	};
public:
	void InitGame();
	virtual int GetTypeID() const { return s_TypeID; }
	virtual hg::IComponent* MakeCopyDerived() const;
	void Update()override;
	void UnPause();
	void SetState(GameState state);
	void OnMessage(hg::Message* msg)override;
public:
	static uint32_t s_TypeID;
private:
	hg::Entity* m_health;
	hg::Entity* m_ammo;
	hg::Entity* m_totalAmmo;
	hg::Entity* m_title;
	hg::Transform* playerPos;
	PlayerComp* player;
	float m_timeScale;
	GameState m_state;
	float m_timer;
	float m_transitionTime;
	float m_transitionTime_inv;
	float m_optionTransTime;
	float m_optionTransTime_inv;
	float m_winFreezeTime;
	hg::Transform* m_desirePosition;
	hg::Camera* m_menuCam;
	XMFLOAT3 m_velocity;
	XMFLOAT3 m_acceleration;
	bool m_First = true;
	std::set<hg::Entity*> m_buttons;
	hg::Entity* m_btnBloom;
	hg::Entity* m_btnVsync;
	bool m_bBloom;
	bool m_bVsync;
private:
	bool Transit();
	bool TransitTo(const MainMenu::GameState& state);
	void PauseGame();
	void ResumeGame();
	void ExitGame();
	void SwitchCamera();
	void HandleControl();
	void RestartGame();
	void UpdateUI();
	void RefreshBTN();
};

