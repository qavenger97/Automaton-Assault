/*!
 * \file DevTextRenderer.h
 *
 * \author Shiyang Ao
 * \date November 2016
 *
 * Developer debug text renderer
 */
#pragma once

namespace Hourglass
{
	class DevTextRenderer
	{
	public:
		static void Init();
		static void DrawText(const char* text, UINT start_x, UINT start_y, const Color& fg = Color(1, 1, 1), const Color& bg = Color(0, 0, 0));

		// Draw text at world position
		static void DrawText_WorldSpace(const char* text, FXMVECTOR worldPos, const Color& fg = Color(1, 1, 1), const Color& bg = Color(0, 0, 0));

		// Draw text at screen position
		static void DrawText_ScreenSpace(const char* text, float screen_x, float screen_y, const Color& fg = Color(1, 1, 1), const Color& bg = Color(0, 0, 0));

		static void Render();
		static void Clear();
	};
}
