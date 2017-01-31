/*!
 * \file DevTextRenderer.cpp
 *
 * \author Shiyang Ao
 * \date November 2016
 *
 * 
 */

#include "pch.h"

#include "DevTextRenderer.h"
#include "Texture.h"
#include "VertexDeclaration.h"
#include "RenderBuffer.h"
#include "RenderDevice.h"
#include "System\CameraSystem.h"

static Hourglass::Texture* s_DevTextFont;
static UINT s_GlyphColumns, s_GlyphRows;
static Hourglass::RenderBuffer s_RenderBuffer;
static std::vector<Hourglass::FontVertex> s_Vertices;
static bool s_NeedUpdateBuffer;

void Hourglass::DevTextRenderer::Init()
{
	s_DevTextFont = g_TextureManager.GetTexture("Assets/Texture/Font/Fixedsys_9c.dds");
	s_GlyphColumns = 16;
	s_GlyphRows = 16;
	s_NeedUpdateBuffer = true;

	ID3D11InputLayout* inputLayout = g_InputLayouts[kRenderShader_Font].Get();
	s_RenderBuffer.CreateVertexBuffer(nullptr, sizeof(FontVertex), 65536, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST, inputLayout, true);
}

void Hourglass::DevTextRenderer::DrawText(const char* text, UINT start_x, UINT start_y, const Color& fg /*= Color(1, 1, 1)*/, const Color& bg /*= Color(0, 0, 0)*/)
{
	float glyph_width = (float)s_DevTextFont->Width / s_GlyphColumns;
	float glyph_height = (float)s_DevTextFont->Height / s_GlyphRows;

	float x0 = glyph_width * start_x, y0 = glyph_height * start_y;

	DrawText_ScreenSpace(text, glyph_width * start_x, glyph_height * start_y, fg, bg);
}

void Hourglass::DevTextRenderer::DrawText_WorldSpace(const char* text, FXMVECTOR worldPos, const Color& fg /*= Color(1, 1, 1)*/, const Color& bg /*= Color(0, 0, 0)*/)
{
	Camera* camera = CameraSystem::GetMainCamera();

	if (!camera)
		return;

	XMVECTOR vPos = XMVectorSetW(worldPos, 1.0f);
	XMVECTOR screenPos = camera->WorldToScreen(vPos);

	if (XMVectorGetZ(screenPos) < 1.0f) // Only draw thing in front of camera
		DevTextRenderer::DrawText_ScreenSpace(text, XMVectorGetX(screenPos), XMVectorGetY(screenPos), fg, bg);
}

void Hourglass::DevTextRenderer::DrawText_ScreenSpace(const char* text, float screen_x, float screen_y, const Color& fg /*= Color(1, 1, 1)*/, const Color& bg /*= Color(0, 0, 0)*/)
{
	float glyph_width = (float)s_DevTextFont->Width / s_GlyphColumns;
	float glyph_height = (float)s_DevTextFont->Height / s_GlyphRows;

	// Avoid glyph 'leaking' by aligning coordinate to pixel 
	float x0 = (float)(int)screen_x, y0 = (float)(int)screen_y;

	for (UINT i = 0; text[i]; i++)
	{
		unsigned char ch = text[i];

		// Handle escape characters
		if (ch == '\n')
		{
			x0 = screen_x;
			y0 += glyph_height;
			continue;
		}

		float u0 = float(ch % s_GlyphRows) / s_GlyphColumns;
		float v0 = float(ch / s_GlyphRows) / s_GlyphRows;
		float u1 = u0 + 1.0f / s_GlyphColumns;
		float v1 = v0 + 1.0f / s_GlyphRows;

		float x1 = x0 + glyph_width;
		float y1 = y0 + glyph_height;

		FontVertex v[6] =
		{
			{ Vector3(x0, y0, 0), fg, bg, Vector2(u0, v0) },
			{ Vector3(x1, y0, 0), fg, bg, Vector2(u1, v0) },
			{ Vector3(x0, y1, 0), fg, bg, Vector2(u0, v1) },

			{ Vector3(x0, y1, 0), fg, bg, Vector2(u0, v1) },
			{ Vector3(x1, y0, 0), fg, bg, Vector2(u1, v0) },
			{ Vector3(x1, y1, 0), fg, bg, Vector2(u1, v1) },
		};

		s_Vertices.insert(s_Vertices.end(), v, v + 6);

		x0 += glyph_width;
	}

	s_NeedUpdateBuffer = true;
}

void Hourglass::DevTextRenderer::Render()
{
	if (s_NeedUpdateBuffer)
	{
		s_RenderBuffer.UpdateDynamicVertexBuffer(s_Vertices.data(), sizeof(FontVertex), (UINT)s_Vertices.size());
		s_NeedUpdateBuffer = false;
	}

	g_RenderDev.deviceContext->VSSetShader(g_RenderShaders[kRenderShader_Font].VertexShader.Get(), nullptr, 0);
	g_RenderDev.deviceContext->PSSetShader(g_RenderShaders[kRenderShader_Font].PixelShader.Get(), nullptr, 0);

	g_RenderDev.deviceContext->PSSetShaderResources(0, 1, s_DevTextFont->ShaderResourceView.GetAddressOf());

	s_RenderBuffer.Draw();
}

void Hourglass::DevTextRenderer::Clear()
{
	s_Vertices.clear();
	s_NeedUpdateBuffer = true;
}

