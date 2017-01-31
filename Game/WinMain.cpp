
#include "GamePch.h"
#include "TechTest.h"
#include "AutomatonAssault.h"

using namespace Hourglass;

int WINAPI WinMain(HINSTANCE instance,
	HINSTANCE prevInstance,
	LPSTR cmdLine, int cmdShow)
{
	//_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	//_CrtSetBreakAlloc(178);	

	Hourglass::Engine* pEngine = new Hourglass::Engine();
	Hourglass::Engine& engine = *pEngine;

	engine.Init();

	AutomatonAssault automatonAssault;
	automatonAssault.Init();

	engine.LoadLevelData();

	engine.Start();

	automatonAssault.Start();

	automatonAssault.CreateScene();

	while (engine.RunOneFrame()) {}

	automatonAssault.Shutdown();
	engine.Shutdown();

	delete pEngine;

	return 0;
}
