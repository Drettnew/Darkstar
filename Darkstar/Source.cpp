#include <Windows.h>
#include "Darkstar.h"
#define _CRTDBG_MAP_ALLOC 

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow) {
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	//_CrtSetBreakAlloc(118289);
	Darkstar* system;

	//Create the engine object
	system = new Darkstar;
	if (!system)
	{
		return 0;
	}

	//Initialize and run the engine
	if (system->Initialize())
	{
		system->Run();
	}

	//Shutdown engine and release the object
	system->Shutdown();
	delete system;
	system = 0;

	return 0;
}