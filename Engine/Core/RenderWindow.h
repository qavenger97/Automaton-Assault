#pragma once

namespace Hourglass
{
	class RenderWindow
	{
	public:
		bool Create(WNDPROC WndProc, int width, int height, bool fullscreen = false, int bpp = 32);
		void Destroy();

		HWND GetHwnd() const { return m_Window; }
		LPCWSTR GetTitle() const;

	private:
		HINSTANCE		m_Instance;			// Win32 application instance
		HWND			m_Window;			// Win32 window handle
		bool			m_bFullScreen;
	};
}
