#pragma once

namespace Hourglass
{
	enum BufferedKeyState
	{
		kKeyStateUp,
		kKeyStateDown,
		kKeyStatePressed,
		kKeyStateReleased,
	};

#define MAX_KEY_NUM 0xFF

	class InputManager
	{
	public:
		// Initialize the input system
		bool Init(HWND hWindow);

		void SetWindowFocus(bool focus);

		// Freeze cursor position
		void LockCursor();

		// Unfreeze cursor position
		void UnlockCursor();

		void ShowCursor();
		void HideCursor();

		void GetCursorPos(int& x, int& y);
		void GetCursorRelPos(int& dx, int& dy);
		void GetCursorWindowPos(int& dx, int& dy);
		BufferedKeyState GetBufferedKeyState(int keycode);
		bool IsKeyDown(int keycode);

		// Set key down event
		//		Called by windows message callback
		void SetKeyDown(int keycode, bool keydown);

		// Update input device states
		//		Called by Engine once per frame
		//		Note: Should be called before window message loop
		void UpdateKeyStates();

	private:
		bool	m_bHasFocus;

		bool	m_bKeyDown[MAX_KEY_NUM];
		bool	m_bKeyDownLastFrame[MAX_KEY_NUM];

		POINT	m_CursorPos;
		POINT	m_CursorPosLastFrame;

		bool	m_bCursorLocked;
		POINT	m_CursorLockingPos;
		HWND	m_hWnd;

		
	};

	extern InputManager g_Input;
}