
#include "pch.h"

#include "InputManager.h"

Hourglass::InputManager Hourglass::g_Input;

bool Hourglass::InputManager::Init(HWND hWindow)
{
	for (int i = 0; i < MAX_KEY_NUM; i++)
	{
		m_bKeyDown[i] = false;
		m_bKeyDownLastFrame[i] = false;
	}

	m_bCursorLocked = false;

	::GetPhysicalCursorPos(&m_CursorPosLastFrame);
	::GetPhysicalCursorPos(&m_CursorPos);
	m_hWnd = hWindow;
	m_bHasFocus = true;
	return true;
}

void Hourglass::InputManager::SetWindowFocus(bool focus)
{
	m_bHasFocus = focus;

	if (focus)
	{
		::GetPhysicalCursorPos(&m_CursorPos);
	}
}

void Hourglass::InputManager::LockCursor()
{
	if (!m_bCursorLocked)
	{
		::GetPhysicalCursorPos(&m_CursorLockingPos);

		m_bCursorLocked = true;
	}
}

void Hourglass::InputManager::UnlockCursor()
{
	if (m_bCursorLocked)
	{
		m_bCursorLocked = false;;
	}
}

void Hourglass::InputManager::ShowCursor()
{
	::ShowCursor(TRUE);
}

void Hourglass::InputManager::HideCursor()
{
	::ShowCursor(FALSE);
}

void Hourglass::InputManager::GetCursorPos(int& x, int& y)
{
	x = m_CursorPos.x;
	y = m_CursorPos.y;
}

void Hourglass::InputManager::GetCursorRelPos(int& dx, int& dy)
{
	dx = m_CursorPos.x - m_CursorPosLastFrame.x;
	dy = m_CursorPos.y - m_CursorPosLastFrame.y;
}

void Hourglass::InputManager::GetCursorWindowPos(int & x, int & y)
{
	POINT pos = { m_CursorPos.x, m_CursorPos.y };
	ScreenToClient(m_hWnd, &pos);
	//ClientToScreen(m_hWnd, &pos);
	x = pos.x;
	y = pos.y;
}	

Hourglass::BufferedKeyState Hourglass::InputManager::GetBufferedKeyState(int keycode)
{
	if (m_bKeyDown[keycode])
	{
		return m_bKeyDownLastFrame[keycode] ? kKeyStateDown : kKeyStatePressed;
	}
	else
	{
		return m_bKeyDownLastFrame[keycode] ? kKeyStateReleased : kKeyStateUp;
	}
}

bool Hourglass::InputManager::IsKeyDown(int keycode)
{
	return m_bKeyDown[keycode];
}

void Hourglass::InputManager::SetKeyDown(int keycode, bool keydown)
{
	m_bKeyDown[keycode] = keydown;
}

void Hourglass::InputManager::UpdateKeyStates()
{
	if (!m_bHasFocus)
		return;

	// Update keyboard key states
	for (int i = 0; i < MAX_KEY_NUM; i++)
	{
		m_bKeyDownLastFrame[i] = m_bKeyDown[i];
	}

	// Update mouse button states
	for (int i = VK_LBUTTON; i <= VK_MBUTTON; i++)
	{
		m_bKeyDown[i] = (GetKeyState(i) & 0x80) != 0;
	}

	// Update system key states
	for (int i = VK_LSHIFT; i <= VK_RMENU; i++)
	{
		m_bKeyDown[i] = (GetKeyState(i) & 0x80) != 0;
	}

	m_CursorPosLastFrame = m_CursorPos;
	::GetPhysicalCursorPos(&m_CursorPos);

	if (m_bCursorLocked)
	{
		::SetPhysicalCursorPos(m_CursorLockingPos.x, m_CursorLockingPos.y);
		m_CursorPosLastFrame = m_CursorLockingPos;
	}
}
