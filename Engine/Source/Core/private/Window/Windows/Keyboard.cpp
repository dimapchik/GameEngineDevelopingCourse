#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <Keyboard.h>
#include <Window/IWindow.h>

namespace GameEngine::Core
{
	bool IsKeyDown(int virtualKey)
	{
		if (!g_MainWindowsApplication || virtualKey < 0x08 || virtualKey > 0xFE) return false;
		const HWND window = static_cast<HWND>(g_MainWindowsApplication->GetWindowHandle());
		if (!window || GetForegroundWindow() != window || IsIconic(window)) return false;
		return (GetAsyncKeyState(virtualKey) & 0x8000) != 0;
	}
}
