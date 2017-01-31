#include "pch.h"
#include "CameraSystem.h"

#include "../Game/GameMessage.h"

Hourglass::Camera* Hourglass::CameraSystem::m_kMainCamera;

int Hourglass::CameraSystem::m_ClientWidth;
int Hourglass::CameraSystem::m_ClientHeight;

namespace Hourglass
{
	CameraSystem g_cameraSystem;
}

void Hourglass::CameraSystem::Init(int width, int height)
{
	INIT_COMPONENT_POOL(Camera);
	m_ClientWidth = width;
	m_ClientHeight = height;
}

void Hourglass::CameraSystem::Update()
{
	for (Camera& c : m_Cameras)
	{
		if (c.IsEnabled() && c.IsAlive())
			c.Update();
	}
}

void Hourglass::CameraSystem::SetMainCamera(Camera* main)
{
	m_kMainCamera = main;
}

Hourglass::Camera * Hourglass::CameraSystem::GetMainCamera()
{
	return m_kMainCamera;
}

Hourglass::Camera * Hourglass::CameraSystem::GetCamera(StrID name)
{
	for (int i = 0; i < 8; i++)
	{
		if (m_Cameras[i].GetEntity())
		{
			if (m_Cameras[i].GetEntity()->GetName() == name)
			{
				return &m_Cameras[i];
			}
		}
	}
	return nullptr;
}

void Hourglass::CameraSystem::OnResize(int width, int height)
{
	m_ClientWidth = width;
	m_ClientHeight = height;

	// TODO: Maybe only notify full screen cameras?
	for (Camera& c : m_Cameras)
	{
		if (c.IsEnabled() && c.IsAlive())
			c.OnMainWindowResize(width, height);
	}
}

void Hourglass::CameraSystem::SendMsg(Message * msg)
{
	if (msg->GetType() == (int)GameMessageType::kPowerUp)
	{
		for (int i = 0; i < s_kMaxCameras; i++)
		{
			if(m_Cameras[i].GetEntity())
				m_Cameras[i].GetEntity()->SendMsg(msg);
		}
	}
}
