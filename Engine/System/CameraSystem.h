#pragma once
#include "ISystem.h"
#include "../Component/Camera.h"
#include "../Component/IComponent.h"
#include "../Core/ComponentPool.h"
namespace Hourglass
{
	class Message;
	class CameraSystem : public ISystem
	{
	public:
		void Init(int width, int height);

		void Update();
		static void SetMainCamera(Camera* main);

		static Camera* GetMainCamera();
		Camera* GetCamera(StrID name);
		void OnResize(int width, int height);
		void SendMsg(hg::Message* msg);
		static int GetScreenWidth() { return m_ClientWidth; }
		static int GetScreenHeight() { return m_ClientHeight; }
	private:
		DECLARE_COMPONENT_POOL(Camera, 8);
		static Camera* m_kMainCamera;

		static int m_ClientWidth;
		static int m_ClientHeight;

	};
	extern CameraSystem g_cameraSystem;
}

