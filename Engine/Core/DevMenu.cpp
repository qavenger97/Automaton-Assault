/*!
 * \file DevMenu.cpp
 *
 * \author Shiyang Ao
 * \date November 2016
 *
 * 
 */

#include "pch.h"

#include "DevMenu.h"
#include "Renderer\DevTextRenderer.h"
#include "InputManager.h"
#include "TimeManager.h"

Hourglass::DevMenu Hourglass::g_DevMenu;

void Hourglass::DevMenu::Init()
{
	m_SelMenu = 0;
	m_Enabled = false;
}

void Hourglass::DevMenu::Shutdown()
{
	for (int i = 0; i < m_MenuItems.size(); i++)
	{
		delete m_MenuItems[i];
	}
}

void Hourglass::DevMenu::AddMenuItem(const char* text)
{
	MenuItem* item = new MenuItem;
	item->text = text;
	m_MenuItems.push_back(item);
}

void Hourglass::DevMenu::Update()
{
#ifdef FINAL
	if (IsKeyDownOrRepeat(VK_F5))
	{
		m_Enabled = !m_Enabled;
	}

	if (!m_Enabled)
		return;

	if (IsKeyDownOrRepeat(VK_DOWN))
	{
		m_SelMenu++;
		m_SelMenu %= m_MenuItems.size();
	}

	if (IsKeyDownOrRepeat(VK_UP))
	{
		if (m_SelMenu == 0)
			m_SelMenu = (UINT)m_MenuItems.size() - 1;
		else
			m_SelMenu--;
	}

	if (IsKeyDownOrRepeat(VK_RIGHT))
	{
		MenuItem* item = m_MenuItems[m_SelMenu];
		item->Increase();
	}

	if (IsKeyDownOrRepeat(VK_LEFT))
	{
		MenuItem* item = m_MenuItems[m_SelMenu];
		item->Decrease();
	}
#endif
}

int Hourglass::DevMenu::Draw(int left /*= 0*/, int top /*= 0*/)
{
	if (!m_Enabled)
		return 0;

	int lines = 0;

	for (int i = 0; i < m_MenuItems.size(); i++)
	{
		if (i == m_SelMenu)
			DevTextRenderer::DrawText(">", max(0, left - 2), top + i);

		DevTextRenderer::DrawText((m_MenuItems[i]->text + m_MenuItems[i]->GetValString()).c_str(), left, top + i);
		lines++;
	}

	return lines;
}

bool Hourglass::DevMenu::IsKeyDownOrRepeat(int keycode)
{
	if (g_Input.GetBufferedKeyState(keycode) == kKeyStatePressed)
	{
		m_KeyRepeatTime = g_Time.Elapsed() + 0.4f;
		return true;
	}

	if ((g_Input.IsKeyDown(keycode) && m_KeyRepeatTime < g_Time.Elapsed()))
	{
		m_KeyRepeatTime = g_Time.Elapsed() + 0.01f;
		return true;
	}

	return false;
}
