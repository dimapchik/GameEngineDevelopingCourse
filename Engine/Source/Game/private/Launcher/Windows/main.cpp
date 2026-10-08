#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files
#include <windows.h>
#include <Window/IWindow.h>
#include <Game.h>
#include <array.h>
#include <iostream>

#if defined(DEBUG) || defined(_DEBUG)
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif

bool WindowsMessageLoop()
{
	MSG msg = { 0 };

	while (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
	{
		if (msg.message == WM_QUIT)
		{
			return false;
		}
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return true;
}

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	// Enable run-time memory check for debug builds.
#if defined(DEBUG) || defined(_DEBUG)
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

	GameEngine::Core::g_MainWindowsApplication = new GameEngine::Core::Window();
	GameEngine::Core::g_MainWindowsApplication->Init(hInstance);

	GameEngine::Core::ControlConfig controls;
	std::wstring executablePath(32768, L'\0');
	const DWORD pathLength = GetModuleFileNameW(nullptr, executablePath.data(), static_cast<DWORD>(executablePath.size()));
	std::string configError;
	if (pathLength > 0 && pathLength < executablePath.size())
	{
		executablePath.resize(pathLength);
		controls.Load(std::filesystem::path(executablePath).parent_path() / L"controls.ini", configError);
	}
	else
	{
		configError = "Cannot resolve executable path; using default controls.";
	}
	if (!configError.empty()) OutputDebugStringA((configError + "\n").c_str());

	std::unique_ptr<GameEngine::Game> game = std::make_unique<GameEngine::Game>(&WindowsMessageLoop, controls);

	game->Run();

	return 0;
}
