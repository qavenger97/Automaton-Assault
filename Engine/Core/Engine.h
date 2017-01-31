#pragma once

#include "Renderer/RenderSystem.h"
#include "../System/AnimationSystem.h"
#include "../System/GameLogicSystem.h"
#include "../System/ParticleSystem.h"
#include "../System/GPUParticleSystem.h"
#include "../System/UISystem.h"
#include "../Core/SceneGraph.h"
#include "../System/PhysicsSystem.h"
#include "../System/AISystem.h"
#include "../System/CameraSystem.h"
#include "../System/MovementControllerSystem.h"
#include "../Core/LevelLoader.h"
#include "System/EventSystem.h"
#include "RenderWindow.h"

namespace Hourglass
{
	class Engine
	{
	public:

		// Initialize engine and all its subsystems
		void Init();

		// Load level data
		void LoadLevelData();

		// Run any post-load initialization
		void Start();

		// Shutdown engine and all its subsystems
		void Shutdown();

		// Update engine for one frame
		//		return true when engine should continue running, false otherwise
		bool RunOneFrame();

		// Resize render window and notify subsystems
		void ResizeClientWindow(int width, int height);

		void ReloadLevel();

		// Create the program main window for rendering
		bool CreateRenderWindow(int width, int height, bool fullscreen = false, int bpp = 32);

		// Destroy the render window before program exits
		void DestroyRenderWindow();
		void OnMessage(int msg);
	private:
		void CalculateFrameStats();
		RenderWindow	m_RenderWindow;

		LevelLoader		m_LevelLoader;
		SceneGraph		m_sceneGraph;
		RenderSystem	m_RenderSystem;
		AnimationSystem m_Animation;
		//ParticleSystem	m_Particles;
		UISystem		m_UI;
		GameLogicSystem m_GameLogic;
		AISystem		m_AI;
		//CameraSystem	m_CameraSystem;
		MovementControllerSystem m_CameraMovementSystem;
		EventSystem		m_EventSystem;

		//gwc::GLog*		m_GLog;
	};
}