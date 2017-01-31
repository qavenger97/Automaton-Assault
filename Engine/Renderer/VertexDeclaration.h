#pragma once

namespace Hourglass
{
	enum VertexDeclType
	{
		kVertexDecl_Color,
		kVertexDecl_Font,
		kVertexDecl_PosUV0NormTan,
		kVertexDecl_PosUV0NormTanSkin,
		kVertexDeclCount,
	};

	struct ColorVertex
	{
		Vector4 pos;
		Vector4 color;
	};

	struct FontVertex
	{
		Vector3 pos;
		Color fg;
		Color bg;
		Vector2 uv;
	};
}