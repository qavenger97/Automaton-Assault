/*!
 * \file DevMenu.h
 *
 * \author Shiyang Ao
 * \date November 2016
 *
 * Developer's menu
 */
#pragma once

namespace Hourglass
{
	class MenuItem
	{
	public:
		std::string	text;

		virtual void Increase() {}
		virtual void Decrease() {}
		virtual std::string GetValString() const { return ""; }
	};

	template <typename T>
	class DevMenuVar : public MenuItem
	{
	public:
		T* Var;
		T Min;
		T Max;
		T Step;

		DevMenuVar(T* pVar, T min, T max, T step)
			: Var(pVar), Min(min), Max(max), Step(step)
		{ }

		void Increase() { if (Var) *Var += min(Step, Max - *Var); }
		void Decrease() { if (Var) *Var -= min(Step, *Var - Min); }

		std::string GetValString() const
		{
			std::stringstream ss;
			ss << *Var;
			return ss.str();
		}
	};

	template<>
	class DevMenuVar<bool> : public MenuItem
	{
	public:
		bool* Var;
		bool Min;
		bool Max;
		bool Step;

		DevMenuVar(bool* pVar, bool min, bool max, bool step)
			: Var(pVar), Min(min), Max(max), Step(step)
		{ }

		void			Increase()				{ if (Var) *Var = !*Var; }
		void			Decrease()				{ if (Var) *Var = !*Var; }

		std::string		GetValString() const	{ return *Var ? "true" : "false"; }
	};

	class DevMenu
	{
	public:
		void Init();

		void Shutdown();

		void AddMenuItem(const char* text);

		template<typename T>
		void AddMenuVar(const char* text, T* pVar, T min, T max, T step)
		{
			MenuItem* item = new DevMenuVar<T>(pVar, min, max, step);
			item->text = text;
			m_MenuItems.push_back(item);
		}

		void AddMenuVarBool(const char* text, bool* pVar)
		{
			AddMenuVar<bool>(text, pVar, false, false, false);
		}

		// Update user input
		void Update();

		// Draw current menu and all it's sub items
		//    Return: total lines drawn
		int Draw(int left = 0, int top = 0);

		void SetEnable(bool enable) { m_Enabled = enable; }
		bool IsEnabled() const { return m_Enabled; }
	private:
		// Check if user presses or holds a key
		bool IsKeyDownOrRepeat(int keycode);

		float m_KeyRepeatTime;

		std::vector<MenuItem*>	m_MenuItems;
		int						m_SelMenu;
		bool					m_Enabled;
	};

	extern DevMenu g_DevMenu;
}
