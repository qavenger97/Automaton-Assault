#include "pch.h"

#include "Engine.h"

#include "InputManager.h"
#include "TimeManager.h"
#include "ComponentFactory.h"
#include "EntityAssembler.h"
#include "System\EntityManager.h"

#include "System\AudioSystemWwise.h"
#include "EngineMessage.h"
#include "Renderer\Texture.h"
#include "Renderer\Material.h"
#include "Animation\AnimationManager.h"
#include "Renderer\DevTextRenderer.h"
#include "Profiler.h"
#include "DebugConst.h"

// Win32 message loop callback function
static LRESULT CALLBACK WndProc( HWND, UINT, WPARAM, LPARAM );

static Hourglass::Engine* s_Engine = nullptr;

void Hourglass::Engine::Init()
{
	s_Engine = this;

	// Notify OS about our DPI awareness so window won't be rescaled by DPI settings
	SetProcessDPIAware();

	int width = 1024;
	int height = 768;
	bool fullscreen = false;

#if 1
	width = GetSystemMetrics( SM_CXSCREEN );
	height = GetSystemMetrics( SM_CYSCREEN );
	fullscreen = true;
#endif

	g_Profiler.Init();

	CreateRenderWindow( width, height, fullscreen );
	g_DevMenu.Init();
	m_RenderSystem.Init( m_RenderWindow.GetHwnd(), width, height, !DEBUG_SHADER );

	g_AudioSystem.Initialize();
	g_AudioSystem.LoadSoundBank( L"Init.bnk" );
	g_AudioSystem.LoadSoundBank( L"GameSoundBank.bnk" );
	g_AudioSystem.LoadSoundBank( L"GameMusic.bnk" );

	m_sceneGraph.Init();
	g_Time.Init();
	g_EntityManager.Init();

	m_AI.Init();
	g_Physics.Init();
	m_UI.Init();
	m_Animation.Init();
	g_particleSystemGPU.Init();
	//m_Particles.Init();
	
	m_CameraMovementSystem.Init();
	g_cameraSystem.Init( width, height );
	g_Input.Init(m_RenderWindow.GetHwnd());

	g_engineMsg.Init( this );
	g_TextureManager.Init();
	g_MaterialManager.Init();
	g_MeshManager.Init();
	g_AnimationManager.Init();

	m_EventSystem.Init();

	DevTextRenderer::Init();

	extern bool g_bDrawDebugDynamicCollider;
	g_DevMenu.AddMenuVarBool( "Debug dynamic collider: ", &g_bDrawDebugDynamicCollider );

	extern bool g_bDrawDebugStaticCollider;
	g_DevMenu.AddMenuVarBool( "Debug static collider: ", &g_bDrawDebugStaticCollider );	

	//gwc::GFile* gfile;
	//gwc::GCreateFile( &gfile );
	//gfile->SetCurrentWorkingDirectory( "../Logs" );
	//gfile->OpenTextWrite( "TestLog.txt" );
	//gwc::GCreateLog( gfile, &m_GLog );
	//gfile->DecrementCount();
}

void Hourglass::Engine::LoadLevelData()
{
	m_AI.LoadBehaviors();
	m_LevelLoader.Init( &m_AI );
}

void Hourglass::Engine::Start()
{
	m_AI.Start();

	m_GameLogic.Start();

	g_particleSystemGPU.Start();
	// Each from the rendering sytem will need poses from the animation system
	// We loosely couple these by providing this reference
	m_RenderSystem.SetPoses( m_Animation.GetPoses() );

	//m_GLog->LogCatergorized( "Category 1", "C1 Msg 0" );	
	//m_GLog->LogCatergorized( "Category 2", "C2 Msg 0" );
	//m_GLog->LogCatergorized( "Category 1", "C1 Msg 1" );
	//m_GLog->LogCatergorized( "Category 2", "C2 Msg 1" );
	//m_GLog->LogCatergorized( "Category 1", "C1 Msg 2" );
}

void Hourglass::Engine::Shutdown()
{
	g_DevMenu.Shutdown();
	g_MeshManager.Shutdown();
	g_MaterialManager.Shutdown();
	g_AnimationManager.Shutdown();
	g_TextureManager.Shutdown();
	m_RenderSystem.Shutdown();
	g_AudioSystem.Shutdown();
	DestroyRenderWindow();
	g_Physics.Shutdown();

	BehaviorFactory::Shutdown();
	g_EntityManager.Shutdown();
	ComponentFactory::Shutdown();
	EntityAssembler::Shutdown();

	s_Engine = nullptr;

	//m_GLog->Flush();
	//m_GLog->DecrementCount();
}

bool Hourglass::Engine::RunOneFrame()
{
	BEGIN_PROFILER();

	BEGIN_PROFILER_BLOCK( "Input" );
	// Update Input
	g_Input.UpdateKeyStates();
	END_PROFILER_BLOCK( "Input" );

	BEGIN_PROFILER_BLOCK( "Win32Message" );
	// Handle Win32 messages

	MSG msg;

	while (PeekMessage( &msg, NULL, 0, 0, PM_REMOVE ))
	{
		if (msg.message == WM_QUIT)
		{
			return false;
		}
		else
		{
			// Translate the message and dispatch it to WndProc()
			TranslateMessage( &msg );
			DispatchMessage( &msg );
		}
	}
	END_PROFILER_BLOCK( "Win32Message" );

	BEGIN_PROFILER_BLOCK( "Timer" );
	g_Time.UpdateFrameTime();
	END_PROFILER_BLOCK( "Timer" );

	// Update Systems
	BEGIN_PROFILER_BLOCK( "Camera" );
	m_CameraMovementSystem.Update();
	g_cameraSystem.Update();
	END_PROFILER_BLOCK( "Camera" );


	BEGIN_PROFILER_BLOCK( "Physics" );
	g_Physics.Update();
	END_PROFILER_BLOCK( "Physics" );

	BEGIN_PROFILER_BLOCK( "SceneGraph" );
	m_sceneGraph.Update();
	END_PROFILER_BLOCK( "SceneGraph" );

	BEGIN_PROFILER_BLOCK( "Animation" );
	m_Animation.Update();
	END_PROFILER_BLOCK( "Animation" );

	//BEGIN_PROFILER_BLOCK( "Particle" );
	//m_Particles.Update();
	//END_PROFILER_BLOCK( "Particle" );

	BEGIN_PROFILER_BLOCK("Particle_GPU");
	g_particleSystemGPU.Update();
	END_PROFILER_BLOCK("Particle_GPU");

	BEGIN_PROFILER_BLOCK( "UI" );
	m_UI.Update();
	END_PROFILER_BLOCK( "UI" );

	BEGIN_PROFILER_BLOCK( "AI" );
	m_AI.Update();
	END_PROFILER_BLOCK( "AI" );

	BEGIN_PROFILER_BLOCK( "GameLogic" );
	m_GameLogic.Update();
	END_PROFILER_BLOCK( "GameLogic" );

	BEGIN_PROFILER_BLOCK("EventSystem");
	m_EventSystem.Update();
	END_PROFILER_BLOCK("EventSystem");

	BEGIN_PROFILER_BLOCK( "Animation - Flatten" );
	m_Animation.FlattenPoses();
	END_PROFILER_BLOCK( "Animation - Flatten" );

	BEGIN_PROFILER_BLOCK( "DevMenu" );
	g_DevMenu.Update();
	END_PROFILER_BLOCK( "DevMenu" );

	BEGIN_PROFILER_BLOCK( "Rendering" );
	m_RenderSystem.Render();
	END_PROFILER_BLOCK( "Rendering" );

	BEGIN_PROFILER_BLOCK( "Audio" );
	g_AudioSystem.Update();
	END_PROFILER_BLOCK( "Audio" );

	BEGIN_PROFILER_BLOCK( "Frame Counter" );
	CalculateFrameStats();
	END_PROFILER_BLOCK( "Frame Counter" );

	END_PROFILER();

	return true;
}

void Hourglass::Engine::ResizeClientWindow( int width, int height )
{
	m_RenderSystem.OnResize( width, height );
	g_cameraSystem.OnResize( width, height );
}

bool Hourglass::Engine::CreateRenderWindow( int width, int height, bool fullscreen /*= false*/, int bpp /*= 32*/ )
{
	return m_RenderWindow.Create(WndProc, width, height, fullscreen, bpp);
}

void Hourglass::Engine::DestroyRenderWindow()
{
	m_RenderWindow.Destroy();
}

void Hourglass::Engine::OnMessage(int msg)
{
	if (msg == MessageType::kToggleBloom)
	{
		m_RenderSystem.TogglePostProcess();
	}
	else if (msg == MessageType::kToggleVsync)
	{
		m_RenderSystem.ToggleVsync();
	}
}

void Hourglass::Engine::CalculateFrameStats()
{
	// Code computes the average frames per second, and also the 
	// average time it takes to render one frame.  These stats 
	// are appended to the window caption bar.

	static int frameCnt = 0;
	static float timeElapsed = 0.0f;

	frameCnt++;

	// Compute averages over one second period.
	if ((g_Time.Elapsed() - timeElapsed) >= 1.0f)
	{
		float fps = (float)frameCnt; // fps = frameCnt / 1
		float mspf = 1000.0f / fps;

		std::wostringstream outs;
		outs.precision( 6 );
		outs << m_RenderWindow.GetTitle() << L"    "
			<< L"FPS: " << fps << L"    "
			<< L"Frame Time: " << mspf << L" (ms)";
		SetWindowText( m_RenderWindow.GetHwnd(), outs.str().c_str() );

		// Reset for next average.
		frameCnt = 0;
		timeElapsed += 1.0f;
	}
}

void Hourglass::Engine::ReloadLevel()
{
	//g_AudioSystem.UnRegisterAllEntities();
	//m_RenderSystem.SetFlags(true);
	m_sceneGraph.Reset();
	g_EntityManager.ResetEntities();
	g_Time.Reset();
	m_LevelLoader.InitLevel(&m_AI);
	m_GameLogic.Start(); 
	g_particleSystemGPU.Reset();
	
}

LRESULT CALLBACK WndProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam )
{
	switch (message)
	{
	case WM_DESTROY:
		// share post quit message with WM_CLOSE
	case WM_CLOSE:
	{
		PostQuitMessage( 0 );
		break;
	}

	case WM_SIZE:
	{
		if (s_Engine)
			s_Engine->ResizeClientWindow( LOWORD( lParam ), HIWORD( lParam ) );
		break;
	}

	case WM_KEYDOWN:
	{
		Hourglass::g_Input.SetKeyDown( (int)wParam, true );
		break;
	}

	case WM_KEYUP:
	{
		Hourglass::g_Input.SetKeyDown( (int)wParam, false );
		break;
	}

	case WM_KILLFOCUS:
	{
		Hourglass::g_Input.SetWindowFocus(false);
		break;
	}

	case WM_SETFOCUS:
	{
		Hourglass::g_Input.SetWindowFocus(true);
		break;
	}

	default:
		return DefWindowProc( hWnd, message, wParam, lParam );
	}

	return 0;
}