#include "GamePch.h"
#include "MainMenu.h"
#include "../Engine/System/CameraSystem.h"
#include "ThirdPersonCameraComponent.h"
#include "PlayerComp.h"
#include "../Engine/Core/Engine.h"
#include "../Engine/Core/EngineMessage.h"
#include "AutomatonAssault.h"
#include "../Engine/System/CameraSystem.h"

uint32_t MainMenu::s_TypeID = hg::ComponentFactory::GetGameComponentID();
void MainMenu::InitGame()
{
	m_bBloom = true;
	m_bVsync = false;
	for (int i = 3; i < 8; i++)
	{
		m_buttons.insert(GetEntity()->GetTransform()->GetChild(i)->GetEntity());
	}
	m_winFreezeTime = 0;
	m_btnVsync = GetEntity()->GetTransform()->GetChild(8)->GetEntity();
	m_btnBloom = GetEntity()->GetTransform()->GetChild(9)->GetEntity();

	player = hg::Entity::FindByName(SID(Player))->GetComponent<PlayerComp>();
	float ratio = hg::CameraSystem::GetMainCamera()->AspectRatio();
	hg::Transform* cameraTrans = hg::g_cameraSystem.GetCamera(SID(Game Camera))->GetEntity()->GetTransform();
	playerPos = hg::Entity::FindByName(SID(PlayerPos))->GetTransform();
	m_desirePosition = hg::Entity::FindByName(SID(DesirePosition))->GetTransform();
	m_health = hg::Entity::FindByName(SID(UI_Health));
	hg::MeshRenderer* mesh = hg::Entity::FindByName(SID(UI_HealthBar))->GetComponent<hg::MeshRenderer>();
	mesh->SetColor(Color(1, 0, 0));
	m_title = hg::Entity::FindByName(SID(UI_TITLE));
	hg::Transform* hpTrans = m_health->GetTransform();
	hpTrans->SetRotation(XMQuaternionConjugate(cameraTrans->GetWorldRotation()));
	hpTrans->Rotate(0, 180 * G_DEG2RAD, 0);
	hpTrans->SetScale(-0.2f, 0.2f, -0.2f);

	m_ammo = hg::Entity::FindByName(SID(UI_Ammo));
	mesh = hg::Entity::FindByName(SID(UI_AmmoBar))->GetComponent<hg::MeshRenderer>();
	mesh->SetColor(Color(0.3f, .8f, 1));

	hg::Transform* ammoTrans = m_ammo->GetTransform();
	ammoTrans->SetRotation(XMQuaternionConjugate(cameraTrans->GetWorldRotation()));
	ammoTrans->Rotate(0, 180 * G_DEG2RAD, 0);
	ammoTrans->SetScale(-0.2f, 0.2f, -0.2f);

	m_totalAmmo = hg::Entity::FindByName(SID(UI_TotalAmmo));
	mesh = hg::Entity::FindByName(SID(UI_TotalAmmoBar))->GetComponent<hg::MeshRenderer>();
	mesh->SetColor(Color(0, 0, 1));

	hg::Transform* totalAmmoTrans = m_totalAmmo->GetTransform();
	totalAmmoTrans->SetRotation(XMQuaternionConjugate(cameraTrans->GetWorldRotation()));
	totalAmmoTrans->Rotate(0, 180 * G_DEG2RAD, 0);
	totalAmmoTrans->SetScale(-0.2f, 0.2f, -0.1f);
	m_timer = 0;
	m_transitionTime = 3.0f;
	m_transitionTime_inv = 1.0f / m_transitionTime;
	m_optionTransTime = 1;
	m_optionTransTime_inv = 1 / m_optionTransTime;
	m_velocity = XMFLOAT3(0, 0, 0);
	m_acceleration = XMFLOAT3(0, 0, 100);
	m_menuCam = hg::g_cameraSystem.GetCamera(SID(Menu Camera));
	PauseGame();
	hg::CameraSystem::GetMainCamera()->GetEntity()->GetTransform()->SetRotation( playerPos->GetWorldRotation() );
}
hg::IComponent * MainMenu::MakeCopyDerived() const
{
	MainMenu* cpy = (MainMenu*)IComponent::Create(SID(MainMenu));
	*cpy = *this;
	return cpy;
}

void MainMenu::Update()
{
#ifdef FINAL
	if (hg::g_Input.GetBufferedKeyState(VK_F1) == hg::BufferedKeyState::kKeyStateReleased)
	{
		SwitchCamera();
	}
#endif
	switch (m_state)
	{
	case MainMenu::MAIN_MENU:
		RefreshBTN();
		if (hg::g_Input.GetBufferedKeyState(VK_ESCAPE) == hg::BufferedKeyState::kKeyStateReleased)
		{
			ExitGame();
		}
		HandleControl();
		break;
	case MainMenu::LOAD_MENU:
		RefreshBTN();
		if (TransitTo(MainMenu::MAIN_MENU))
			SetState(MainMenu::PAUSE);
		break;
	case MainMenu::LOAD_OPTION:
		RefreshBTN();
		if (TransitTo(MainMenu::OPTION))
			SetState(MainMenu::OPTION);
		break;
	case MainMenu::OPTION:
		RefreshBTN();
		if (hg::g_Input.GetBufferedKeyState(VK_ESCAPE) == hg::BufferedKeyState::kKeyStateReleased)
		{
			SetState(MainMenu::LOAD_MENU);
		}
		HandleControl();
		break;
	case MainMenu::LOADING:
		RefreshBTN();
		if (Transit())
		{
			m_title->GetComponent<hg::MeshRenderer>()->SetMesh(hg::g_MeshManager.GetMesh("Assets/Mesh/UI_TITLE.hmdl"));
			ResumeGame();
		}
		break;
	case MainMenu::RUNNING:
		if (hg::g_Input.GetBufferedKeyState(VK_ESCAPE) == hg::BufferedKeyState::kKeyStateReleased)
		{
			hg::CameraSystem::SetMainCamera(hg::g_cameraSystem.GetCamera(SID(Menu Camera)));
			hg::CameraSystem::GetMainCamera()->GetEntity()->GetTransform()->SetPosition(playerPos->GetWorldPosition());
			hg::CameraSystem::GetMainCamera()->GetEntity()->GetTransform()->SetRotation(playerPos->GetWorldRotation());
			PauseGame();
			SetState(GameState::PAUSE);
		}

#ifdef FINAL
		if (hg::g_Input.GetBufferedKeyState(VK_F2) == hg::BufferedKeyState::kKeyStateReleased)
		{
			if (hg::g_Time.GetTimeScale())
			{
				PauseGame();
			}
			else
			{
				UnPause();
			}
		}
#endif

		UpdateUI();
		break;
	case MainMenu::PAUSE:
		RefreshBTN();
		if (hg::g_Input.GetBufferedKeyState(VK_ESCAPE) == hg::BufferedKeyState::kKeyStateReleased)
		{
			ResumeGame();
		}
		HandleControl();
		break;
	case MainMenu::GAMEOVER:
		RefreshBTN();
		if (hg::g_Input.GetBufferedKeyState(VK_ESCAPE) == hg::BufferedKeyState::kKeyStateReleased)
		{
			ExitGame();
		}
		HandleControl();
		break;
	case MainMenu::WIN:
	{
		m_winFreezeTime -= hg::g_Time.UnscaledDelta();
		for (auto* o : m_buttons)
		{
			o->GetComponent<hg::MeshRenderer>()->SetEnabled(false);
		}
		if (m_winFreezeTime <= 0)
		{
			for (auto* o : m_buttons)
			{
				o->GetComponent<hg::MeshRenderer>()->SetEnabled(true);
			}
			SetState(GameState::MAIN_MENU);
		}
	}
		
		break;
	default:
		break;
	}
}

void MainMenu::UnPause()
{
	hg::g_Time.SetTimeScale(1);
}

void MainMenu::RefreshBTN()
{
	for (auto* e : m_buttons)
	{
		e->GetComponent<hg::MeshRenderer>()->SetMaterial(hg::g_MaterialManager.GetMaterial("Assets/Material/PBR/PBR_BTN.xml"));
	}
	if (m_bBloom)
	{
		m_btnBloom->GetComponent<hg::MeshRenderer>()->SetMaterial(hg::g_MaterialManager.GetMaterial("Assets/Material/PBR/PBR_VatGlow.xml"));
		m_btnBloom->GetComponent<hg::MeshRenderer>()->SetMesh(hg::g_MeshManager.GetMesh("Assets/Mesh/UI_Bloom_On.hmdl"));
	}
	else
	{
		m_btnBloom->GetComponent<hg::MeshRenderer>()->SetMaterial(hg::g_MaterialManager.GetMaterial("Assets/Material/PBR/PBR_Cyan.xml"));
		m_btnBloom->GetComponent<hg::MeshRenderer>()->SetMesh(hg::g_MeshManager.GetMesh("Assets/Mesh/UI_Bloom_Off.hmdl"));
	}

	if (m_bVsync)
	{
		m_btnVsync->GetComponent<hg::MeshRenderer>()->SetMaterial(hg::g_MaterialManager.GetMaterial("Assets/Material/PBR/PBR_VatGlow.xml"));
		m_btnVsync->GetComponent<hg::MeshRenderer>()->SetMesh(hg::g_MeshManager.GetMesh("Assets/Mesh/UI_VSync_On.hmdl"));
	}
	else
	{
		m_btnVsync->GetComponent<hg::MeshRenderer>()->SetMaterial(hg::g_MaterialManager.GetMaterial("Assets/Material/PBR/PBR_Cyan.xml"));
		m_btnVsync->GetComponent<hg::MeshRenderer>()->SetMesh(hg::g_MeshManager.GetMesh("Assets/Mesh/UI_VSync_Off.hmdl"));
	}
}

bool MainMenu::Transit()
{
	if (m_timer >= m_transitionTime)
	{
		m_timer = 0;
		m_First = false;
		return true;
	}

	hg::Transform* transform = m_menuCam->GetEntity()->GetTransform();
	if (m_timer < 1)
	{
		float ratio = m_timer * m_transitionTime_inv * 3;
		XMVECTOR pos = XMVectorLerp(playerPos->GetWorldPosition(), m_desirePosition->GetWorldPosition(), ratio);
		XMVECTOR rot = XMQuaternionSlerp(playerPos->GetWorldRotation(), m_desirePosition->GetWorldRotation(), ratio);
	
		transform->SetPosition(pos);
		transform->SetRotation(rot);
	}
	else
	{
		XMVECTOR pos = m_menuCam->GetEntity()->GetTransform()->GetWorldPosition();
		XMVECTOR vel = XMLoadFloat3(&m_velocity);
		XMVECTOR acc = XMLoadFloat3(&m_acceleration);
		Math::Integrate(pos, vel, hg::g_Time.UnscaledDelta(), acc);
		transform->SetPosition(pos);
		XMStoreFloat3(&m_velocity, vel);
		m_acceleration.z += 600 * hg::g_Time.UnscaledDelta();
	}

	m_timer += hg::g_Time.UnscaledDelta();
	return false;
}

bool MainMenu::TransitTo(const MainMenu::GameState & state)
{
	hg::Transform* transform = m_menuCam->GetEntity()->GetTransform();
	if (m_timer >= m_optionTransTime)
	{
		playerPos->SetRotation(transform->GetWorldRotation());
		m_timer = 0;
		return true;
	}

	XMVECTOR desire = XMQuaternionMultiply(
		XMQuaternionRotationRollPitchYaw(state == GameState::MAIN_MENU ? 89.999999f * G_DEG2RAD : -89.999999f * G_DEG2RAD, 0, 0),
		playerPos->GetWorldRotation());
	float ratio = m_timer * m_optionTransTime_inv;
	XMVECTOR rot = XMQuaternionSlerp(playerPos->GetWorldRotation(), desire, ratio);
	m_timer += hg::g_Time.UnscaledDelta();

	transform->SetRotation(rot);

	return false;
}

void MainMenu::PauseGame()
{
	m_timeScale = hg::g_Time.GetTimeScale();
	hg::g_Time.SetTimeScale(0);
}

void MainMenu::ResumeGame()
{
	hg::CameraSystem::SetMainCamera(hg::g_cameraSystem.GetCamera(SID(Game Camera)));
	UnPause();
	SetState(GameState::RUNNING);
}

void MainMenu::ExitGame()
{
	PostQuitMessage(0);
}

void MainMenu::SwitchCamera()
{
	hg::Camera* freeCam;
	hg::Camera* cam;
	freeCam = hg::g_cameraSystem.GetCamera(SID(Free Camera));
	switch (m_state)
	{
	case MainMenu::PAUSE:
	case MainMenu::MAIN_MENU:
	case MainMenu::LOAD_MENU:
	case MainMenu::OPTION:
	case MainMenu::LOAD_OPTION:
		cam = hg::g_cameraSystem.GetCamera(SID(Menu Camera));
		break;
	case MainMenu::LOADING:
		return;
		break;
	case MainMenu::RUNNING:
		cam = hg::g_cameraSystem.GetCamera(SID(Game Camera));
		break;
	}
	if (hg::g_cameraSystem.GetMainCamera() == freeCam)
	{
		hg::g_cameraSystem.SetMainCamera(cam);
		UnPause();
	}
	else
	{
		hg::g_cameraSystem.SetMainCamera(freeCam);
		PauseGame();
	}
	freeCam->GetEntity()->GetTransform()->SetPosition(cam->GetEntity()->GetTransform()->GetWorldPosition());
	freeCam->GetEntity()->GetTransform()->SetRotation(cam->GetEntity()->GetTransform()->GetWorldRotation());
}

void MainMenu::HandleControl()
{
	int x, y;
	hg::g_Input.GetCursorWindowPos(x, y);
	XMVECTOR pos_mouse_world = XMVectorSet((float)x, (float)y, 1, 1);
	hg::Ray ray = hg::CameraSystem::GetMainCamera()->ScreenToWorldRay(pos_mouse_world);
	ray.Distance = 100;
	hg::Entity* hit = nullptr;
	hg::g_Physics.RayCast(ray, &hit);
	if (hit)
	{
		if (m_buttons.find(hit) != m_buttons.end())
		{
			hit->GetComponent<hg::MeshRenderer>()->SetMaterial(hg::g_MaterialManager.GetMaterial("Assets/Material/PBR/PBR_BTN_On.xml"));
		}
		else if (hit == m_btnBloom || hit == m_btnVsync)
		{
			hit->GetComponent<hg::MeshRenderer>()->SetMaterial(hg::g_MaterialManager.GetMaterial("Assets/Material/PBR/PBR_BTN_On.xml"));
		}
	}
	if (hg::g_Input.GetBufferedKeyState(VK_LBUTTON) == hg::BufferedKeyState::kKeyStateReleased)
	{
		if (hit)
		{
			StrID name = hit->GetName();
			if (name == SID(BTN_NewGame))
			{
				RestartGame();
			}
			else if (name == SID(BTN_Option))
			{
				SetState(GameState::LOAD_OPTION);
			}
			else if (name == SID(BTN_BACK))
			{
				SetState(GameState::LOAD_MENU);
			}
			else if (name == SID(BTN_Exit))
			{
				ExitGame();
			}
			else if (name == SID(BTN_Restart))
			{
				RestartGame();
			}
			else if (name == SID(BTN_VSYNC))
			{
				m_bVsync = !m_bVsync;
				hg::g_engineMsg.SendMsg(hg::MessageType::kToggleVsync);
			}
			else if (name == SID(BTN_BLOOM))
			{
				m_bBloom = !m_bBloom;
				hg::g_engineMsg.SendMsg(hg::MessageType::kToggleBloom);
			}
		}
	}
}

void MainMenu::RestartGame()
{
	hg::g_engineMsg.SendMsg(hg::MessageType::kResetLevel);
	AutomatonAssault::CreateScene();
	SetState(GameState::LOADING);
}

void MainMenu::UpdateUI()
{
	hg::Transform* cameraTrans = hg::g_cameraSystem.GetCamera(SID(Game Camera))->GetEntity()->GetTransform();
	float ratio = hg::CameraSystem::GetMainCamera()->AspectRatio();
	XMVECTOR pos = XMVectorSet(-2.0f*ratio, -1.5f, 3, 0) + cameraTrans->GetWorldPosition();
	
	hg::Transform* hpTrans = m_health->GetTransform();
	hpTrans->SetPosition(pos);
	hpTrans->UpdateChildren();

	pos += XMVectorSet(0, -0.3f, 0, 0);

	hg::Transform* ammoTrans = m_ammo->GetTransform();
	ammoTrans->SetPosition(pos);
	ammoTrans->UpdateChildren();

	pos += XMVectorSet(-0.05f, -0.2f, -0.005f, 0);

	hg::Transform* totalAmmoTrans = m_totalAmmo->GetTransform();
	totalAmmoTrans->SetPosition(pos);
	totalAmmoTrans->UpdateChildren();

	ammoTrans->GetChild(0)->SetScale(-0.2f*player->GetCurrentAmmoRatio(), 0.2f, -0.2f);
	hpTrans->GetChild(0)->SetScale(-0.2f*player->GetCurrentHealthRatio(), 0.2f, -0.2f);
	totalAmmoTrans->GetChild(0)->SetScale(-0.2f*player->GetCurrentTotalAmmoRatio(), 0.2f, -0.1f);

}

void MainMenu::SetState(GameState state)
{
	if (!m_First)
	{
		hg::Entity* restart = hg::Entity::FindByName(SID(BTN_Restart));
		hg::Entity* newGame = hg::Entity::FindByName(SID(BTN_NewGame));
		restart->SetEnabled(true);
		restart->GetComponent<hg::MeshRenderer>()->SetEnabled(true);
		restart->GetComponent<hg::DynamicCollider>()->SetEnabled(true);

		newGame->SetEnabled(false);
		newGame->GetComponent<hg::MeshRenderer>()->SetEnabled(false);
		newGame->GetComponent<hg::DynamicCollider>()->SetEnabled(false);
	}
	else
	{
		hg::Entity* restart = hg::Entity::FindByName(SID(BTN_Restart));
		hg::Entity* newGame = hg::Entity::FindByName(SID(BTN_NewGame));
		restart->SetEnabled(false);
		restart->GetComponent<hg::MeshRenderer>()->SetEnabled(false);
		restart->GetComponent<hg::DynamicCollider>()->SetEnabled(false);

		newGame->SetEnabled(true);
		newGame->GetComponent<hg::MeshRenderer>()->SetEnabled(true);
		newGame->GetComponent<hg::DynamicCollider>()->SetEnabled(true);

	}
	m_state = state;
}

void MainMenu::OnMessage(hg::Message * msg)
{
	int msgType = msg->GetType();
	if (msgType == (int)GameMessageType::kPlayerDeath)
	{
		hg::CameraSystem::SetMainCamera(hg::g_cameraSystem.GetCamera(SID(Menu Camera)));
		hg::CameraSystem::GetMainCamera()->GetEntity()->GetTransform()->SetPosition(playerPos->GetWorldPosition());
		hg::CameraSystem::GetMainCamera()->GetEntity()->GetTransform()->SetRotation(playerPos->GetWorldRotation());
		PauseGame();
		SetState(GameState::GAMEOVER);
	}
	else if (msgType == GameMessageType::kWin)
	{
		hg::CameraSystem::SetMainCamera(hg::g_cameraSystem.GetCamera(SID(Menu Camera)));
		hg::CameraSystem::GetMainCamera()->GetEntity()->GetTransform()->SetPosition(playerPos->GetWorldPosition());
		hg::CameraSystem::GetMainCamera()->GetEntity()->GetTransform()->SetRotation(playerPos->GetWorldRotation());
		m_title->GetComponent<hg::MeshRenderer>()->SetMesh(hg::g_MeshManager.GetMesh("Assets/Mesh/UI_WIN.hmdl"));
		PauseGame();
		m_First = true;
		m_winFreezeTime = 2;
		hg::g_AudioSystem.PostEvent(AK::EVENTS::STOP_GAMEMUSIC);
		hg::g_AudioSystem.PostEvent(AK::EVENTS::STOP_BOSSMUSIC);
		hg::g_AudioSystem.PostEvent(AK::EVENTS::STOP_BOSSMUSICINTRO);
		hg::g_AudioSystem.SetState(AK::STATES::ATMOSPHERE::GROUP, AK::STATES::ATMOSPHERE::STATE::SAFE);
		hg::g_AudioSystem.SetState(AK::STATES::ENEMY::GROUP, AK::STATES::ENEMY::STATE::DEFAULT);
		hg::g_AudioSystem.PostEvent(AK::EVENTS::PLAY_GAMEMUSIC);

		SetState(GameState::WIN);
	}
	else if (msgType == GameMessageType::kPowerUp)
	{
		UINT flag = ((PowerUpMessage*)msg)->GetPowerUpType();
		if (flag & PowerUpType::kPowerUp_End)
		{
			if (flag & PowerUpType::kPowerUp_Ricochet)
			{
				m_ammo->GetTransform()->FindChild(SID(UI_AmmoBar))->GetEntity()->GetComponent<hg::MeshRenderer>()->SetMaterial(hg::g_MaterialManager.GetMaterial("Assets/Material/PBR/PBR_Emissive_blue.xml"));
			}
			if (flag & PowerUpType::kPowerUp_Shield)
			{
				m_health->GetTransform()->FindChild(SID(UI_HealthBar))->GetEntity()->GetComponent<hg::MeshRenderer>()->SetMaterial(hg::g_MaterialManager.GetMaterial("Assets/Material/PBR/PBR_Emissive_red.xml"));
			}
		}
		else if (flag & PowerUpType::kPowerUp_Ricochet)
		{
			m_ammo->GetTransform()->FindChild(SID(UI_AmmoBar))->GetEntity()->GetComponent<hg::MeshRenderer>()->SetMaterial(hg::g_MaterialManager.GetMaterial("Assets/Material/PBR/PBR_Light_Yellow.xml"));
		}
		else if (flag & PowerUpType::kPowerUp_Shield)
		{
			m_health->GetTransform()->FindChild(SID(UI_HealthBar))->GetEntity()->GetComponent<hg::MeshRenderer>()->SetMaterial(hg::g_MaterialManager.GetMaterial("Assets/Material/PBR/PBR_BTN_On.xml"));
		}
	}
}
