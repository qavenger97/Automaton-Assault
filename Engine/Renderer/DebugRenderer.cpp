#include "pch.h"

#include "DebugRenderer.h"
#include "RenderBuffer.h"
#include "VertexDeclaration.h"
#include "RenderDevice.h"

// Color for next debug primitive
static Vector4 s_PrimitiveColor = Vector4(1.0f, 1.0f, 1.0f, 1.0f);

// Render buffer used by debug renderer
static Hourglass::RenderBuffer s_DebugRendererBuffer;

static std::vector<Hourglass::ColorVertex> s_DebugRendererPrimitives;

static bool s_NeedsUpdateBuffer = false;

static const int s_VertexBufferSize = 65536;

void Hourglass::DebugRenderer::Init()
{
	s_DebugRendererBuffer.CreateVertexBuffer(nullptr, sizeof(ColorVertex), s_VertexBufferSize, D3D11_PRIMITIVE_TOPOLOGY_LINELIST, g_InputLayouts[kVertexDecl_Color].Get(), true);
}

void Hourglass::DebugRenderer::SetLineColor(const Vector4& color)
{
	s_PrimitiveColor = color;
}

Vector4 Hourglass::DebugRenderer::GetLineColor()
{
	return s_PrimitiveColor;
}

void Hourglass::DebugRenderer::DrawLine(const Vector3& v1, const Vector3& v2)
{
	DrawLine(v1, v2, s_PrimitiveColor, s_PrimitiveColor);
}

void Hourglass::DebugRenderer::DrawLine(const Vector3& v1, const Vector3& v2, const Vector4& color)
{
	DrawLine(v1, v2, color, color);
}

void Hourglass::DebugRenderer::DrawLine(const Vector3& v1, const Vector3& v2, const Vector4& color1, const Vector4& color2)
{
	s_DebugRendererPrimitives.push_back({ Vector4(v1.x, v1.y, v1.z, 1.0f), color1 });
	s_DebugRendererPrimitives.push_back({ Vector4(v2.x, v2.y, v2.z, 1.0f), color2 });

	s_NeedsUpdateBuffer = true;
}

void Hourglass::DebugRenderer::DrawAabb(const Aabb& aabb)
{
	DrawAabb(aabb, s_PrimitiveColor);
}

void Hourglass::DebugRenderer::DrawAxis(const Transform& transform)
{
	XMVECTOR pos = transform.GetWorldPosition();
	XMVECTOR forward_vec =	XMVectorAdd(pos, transform.Forward());
	XMVECTOR right_vec =	XMVectorAdd(pos, transform.Right());
	XMVECTOR up_vec =		XMVectorAdd(pos, transform.Up());
	DrawLine(pos, right_vec,	Color(1, 0, 0));
	DrawLine(pos, up_vec,		Color(0, 1, 0));
	DrawLine(pos, forward_vec,	Color(0, 0, 1));
}

void Hourglass::DebugRenderer::DrawAxis(const Matrix& m, float scale)
{
	Vector3 pos = m.Translation();
	Vector3 forward_vec = pos + m.Forward() * scale;
	Vector3 right_vec = pos + m.Right() * scale;
	Vector3 up_vec = pos + m.Up() * scale;
	DrawLine(pos, right_vec,	Color(1, 0, 0));
	DrawLine(pos, up_vec,		Color(0, 1, 0));
	DrawLine(pos, forward_vec,	Color(0, 0, 1));
}

void Hourglass::DebugRenderer::DrawAabb(const Aabb& aabb, const Vector4& color)
{
#if 0
	Vector4 cornerPoints[] =
	{
		Vector4(aabb.pMin.x, aabb.pMin.y, aabb.pMin.z, 1.0f),
		Vector4(aabb.pMin.x, aabb.pMin.y, aabb.pMax.z, 1.0f),
		Vector4(aabb.pMin.x, aabb.pMax.y, aabb.pMax.z, 1.0f),
		Vector4(aabb.pMin.x, aabb.pMax.y, aabb.pMin.z, 1.0f),

		Vector4(aabb.pMax.x, aabb.pMin.y, aabb.pMin.z, 1.0f),
		Vector4(aabb.pMax.x, aabb.pMin.y, aabb.pMax.z, 1.0f),
		Vector4(aabb.pMax.x, aabb.pMax.y, aabb.pMax.z, 1.0f),
		Vector4(aabb.pMax.x, aabb.pMax.y, aabb.pMin.z, 1.0f),
	};

	int wiredCubeIdx[] =
	{
		0, 1, 1, 2, 2, 3, 3, 0,
		4, 5, 5, 6, 6, 7, 7, 4,
		0, 4, 1, 5, 2, 6, 3, 7,
	};
	int i = 0;
	const int size = 24;
	for (; i < size; i++)
	{
		ColorVertex v =
		{
			cornerPoints[wiredCubeIdx[i]],
			color,
		};
		s_DebugRendererPrimitives.push_back(v);
	}
#else
	ColorVertex v[] =
	{
		{ Vector4(aabb.pMin.x, aabb.pMin.y, aabb.pMin.z, 1.0f), color },
		{ Vector4(aabb.pMin.x, aabb.pMin.y, aabb.pMax.z, 1.0f), color },
		{ Vector4(aabb.pMin.x, aabb.pMin.y, aabb.pMax.z, 1.0f), color },
		{ Vector4(aabb.pMin.x, aabb.pMax.y, aabb.pMax.z, 1.0f), color },
		{ Vector4(aabb.pMin.x, aabb.pMax.y, aabb.pMax.z, 1.0f), color },
		{ Vector4(aabb.pMin.x, aabb.pMax.y, aabb.pMin.z, 1.0f), color },
		{ Vector4(aabb.pMin.x, aabb.pMax.y, aabb.pMin.z, 1.0f), color },
		{ Vector4(aabb.pMin.x, aabb.pMin.y, aabb.pMin.z, 1.0f), color },
		{ Vector4(aabb.pMax.x, aabb.pMin.y, aabb.pMin.z, 1.0f), color },
		{ Vector4(aabb.pMax.x, aabb.pMin.y, aabb.pMax.z, 1.0f), color },
		{ Vector4(aabb.pMax.x, aabb.pMin.y, aabb.pMax.z, 1.0f), color },
		{ Vector4(aabb.pMax.x, aabb.pMax.y, aabb.pMax.z, 1.0f), color },
		{ Vector4(aabb.pMax.x, aabb.pMax.y, aabb.pMax.z, 1.0f), color },
		{ Vector4(aabb.pMax.x, aabb.pMax.y, aabb.pMin.z, 1.0f), color },
		{ Vector4(aabb.pMax.x, aabb.pMax.y, aabb.pMin.z, 1.0f), color },
		{ Vector4(aabb.pMax.x, aabb.pMin.y, aabb.pMin.z, 1.0f), color },
		{ Vector4(aabb.pMin.x, aabb.pMin.y, aabb.pMin.z, 1.0f), color },
		{ Vector4(aabb.pMax.x, aabb.pMin.y, aabb.pMin.z, 1.0f), color },
		{ Vector4(aabb.pMin.x, aabb.pMin.y, aabb.pMax.z, 1.0f), color },
		{ Vector4(aabb.pMax.x, aabb.pMin.y, aabb.pMax.z, 1.0f), color },
		{ Vector4(aabb.pMin.x, aabb.pMax.y, aabb.pMax.z, 1.0f), color },
		{ Vector4(aabb.pMax.x, aabb.pMax.y, aabb.pMax.z, 1.0f), color },
		{ Vector4(aabb.pMin.x, aabb.pMax.y, aabb.pMin.z, 1.0f), color },
		{ Vector4(aabb.pMax.x, aabb.pMax.y, aabb.pMin.z, 1.0f), color },
	};

	s_DebugRendererPrimitives.insert(s_DebugRendererPrimitives.end(), v, v + 24);
#endif
	s_NeedsUpdateBuffer = true;
}

void Hourglass::DebugRenderer::DrawAabb(const Aabb& aabb, const Matrix& m, const Vector4& color)
{
	ColorVertex v[] =
	{
		{ Vector4(aabb.pMin.x, aabb.pMin.y, aabb.pMin.z, 1.0f), color },
		{ Vector4(aabb.pMin.x, aabb.pMin.y, aabb.pMax.z, 1.0f), color },
		{ Vector4(aabb.pMin.x, aabb.pMin.y, aabb.pMax.z, 1.0f), color },
		{ Vector4(aabb.pMin.x, aabb.pMax.y, aabb.pMax.z, 1.0f), color },
		{ Vector4(aabb.pMin.x, aabb.pMax.y, aabb.pMax.z, 1.0f), color },
		{ Vector4(aabb.pMin.x, aabb.pMax.y, aabb.pMin.z, 1.0f), color },
		{ Vector4(aabb.pMin.x, aabb.pMax.y, aabb.pMin.z, 1.0f), color },
		{ Vector4(aabb.pMin.x, aabb.pMin.y, aabb.pMin.z, 1.0f), color },
		{ Vector4(aabb.pMax.x, aabb.pMin.y, aabb.pMin.z, 1.0f), color },
		{ Vector4(aabb.pMax.x, aabb.pMin.y, aabb.pMax.z, 1.0f), color },
		{ Vector4(aabb.pMax.x, aabb.pMin.y, aabb.pMax.z, 1.0f), color },
		{ Vector4(aabb.pMax.x, aabb.pMax.y, aabb.pMax.z, 1.0f), color },
		{ Vector4(aabb.pMax.x, aabb.pMax.y, aabb.pMax.z, 1.0f), color },
		{ Vector4(aabb.pMax.x, aabb.pMax.y, aabb.pMin.z, 1.0f), color },
		{ Vector4(aabb.pMax.x, aabb.pMax.y, aabb.pMin.z, 1.0f), color },
		{ Vector4(aabb.pMax.x, aabb.pMin.y, aabb.pMin.z, 1.0f), color },
		{ Vector4(aabb.pMin.x, aabb.pMin.y, aabb.pMin.z, 1.0f), color },
		{ Vector4(aabb.pMax.x, aabb.pMin.y, aabb.pMin.z, 1.0f), color },
		{ Vector4(aabb.pMin.x, aabb.pMin.y, aabb.pMax.z, 1.0f), color },
		{ Vector4(aabb.pMax.x, aabb.pMin.y, aabb.pMax.z, 1.0f), color },
		{ Vector4(aabb.pMin.x, aabb.pMax.y, aabb.pMax.z, 1.0f), color },
		{ Vector4(aabb.pMax.x, aabb.pMax.y, aabb.pMax.z, 1.0f), color },
		{ Vector4(aabb.pMin.x, aabb.pMax.y, aabb.pMin.z, 1.0f), color },
		{ Vector4(aabb.pMax.x, aabb.pMax.y, aabb.pMin.z, 1.0f), color },
	};

	for (int i = 0; i < 24; i++)
	{
		v[i].pos = Vector4::Transform(v[i].pos, m);
	}

	s_DebugRendererPrimitives.insert(s_DebugRendererPrimitives.end(), v, v + 24);

	s_NeedsUpdateBuffer = true;
}

void Hourglass::DebugRenderer::DrawSphere(const Vector3& center, float radius, int segment /*= 12*/)
{
	DrawSphere(center, radius, s_PrimitiveColor, segment);
}

void Hourglass::DebugRenderer::DrawSphere(const Vector3& center, float radius, const Vector4& color, int segment /*= 12*/)
{
	for (int i = 0; i < segment; i++)
	{
		for (int j = 1; j < segment; j++)
		{
			float y = cosf(DirectX::XM_PI * j / segment) * radius + center.y;
			float r = sinf(DirectX::XM_PI * j / segment) * radius;

			float x0 = sinf(DirectX::XM_2PI * i / segment) * r + center.x;
			float z0 = cosf(DirectX::XM_2PI * i / segment) * r + center.z;

			int i1 = (i + 1) % segment;
			float x1 = sinf(DirectX::XM_2PI * i1 / segment) * r + center.x;
			float z1 = cosf(DirectX::XM_2PI * i1 / segment) * r + center.z;

			ColorVertex v0 = { Vector4(x0, y, z0, 1.0f), color };
			ColorVertex v1 = { Vector4(x1, y, z1, 1.0f), color };
			s_DebugRendererPrimitives.push_back(v0);
			s_DebugRendererPrimitives.push_back(v1);
		}
	}

	for (int i = 0; i < segment; i++)
	{
		for (int j = 0; j < segment; j++)
		{
			float y0 = cosf(DirectX::XM_PI * j / segment) * radius + center.y;
			float r0 = sinf(DirectX::XM_PI * j / segment) * radius;

			int j1 = j + 1;
			float y1 = cosf(DirectX::XM_PI * j1 / segment) * radius + center.y;
			float r1 = sinf(DirectX::XM_PI * j1 / segment) * radius;

			float x0 = sinf(DirectX::XM_2PI * i / segment) * r0 + center.x;
			float z0 = cosf(DirectX::XM_2PI * i / segment) * r0 + center.z;

			float x1 = sinf(DirectX::XM_2PI * i / segment) * r1 + center.x;
			float z1 = cosf(DirectX::XM_2PI * i / segment) * r1 + center.z;

			ColorVertex v0 = { Vector4(x0, y0, z0, 1.0f), color };
			ColorVertex v1 = { Vector4(x1, y1, z1, 1.0f), color };
			s_DebugRendererPrimitives.push_back(v0);
			s_DebugRendererPrimitives.push_back(v1);
		}
	}

	s_NeedsUpdateBuffer = true;
}

void Hourglass::DebugRenderer::DrawCylinder(const Cylinder& cylinder, const Vector4& color, int segment /*= 12*/)
{
	for (int i = 0; i < segment; i++)
	{
		float y0 = cylinder.Center.y + 0.5f * cylinder.Height;
		float y1 = cylinder.Center.y - 0.5f * cylinder.Height;
		float r = cylinder.Radius;

		float x0 = sinf(DirectX::XM_2PI * i / segment) * r + cylinder.Center.x;
		float z0 = cosf(DirectX::XM_2PI * i / segment) * r + cylinder.Center.z;

		int i1 = (i + 1) % segment;
		float x1 = sinf(DirectX::XM_2PI * i1 / segment) * r + cylinder.Center.x;
		float z1 = cosf(DirectX::XM_2PI * i1 / segment) * r + cylinder.Center.z;

		ColorVertex v0 = { Vector4(x0, y0, z0, 1.0f), color };
		ColorVertex v1 = { Vector4(x1, y0, z1, 1.0f), color };
		ColorVertex v2 = { Vector4(x0, y1, z0, 1.0f), color };
		ColorVertex v3 = { Vector4(x1, y1, z1, 1.0f), color };
		s_DebugRendererPrimitives.push_back(v0);
		s_DebugRendererPrimitives.push_back(v1);
		s_DebugRendererPrimitives.push_back(v0);
		s_DebugRendererPrimitives.push_back(v2);
		s_DebugRendererPrimitives.push_back(v2);
		s_DebugRendererPrimitives.push_back(v3);
	}

	s_NeedsUpdateBuffer = true;
}

void Hourglass::DebugRenderer::DrawFrustum(const BoundingFrustum& frustum, const Color& color)
{
	XMFLOAT3 corners[BoundingFrustum::CORNER_COUNT];
	frustum.GetCorners(corners);

	ColorVertex verts[24] = {};
	verts[0].pos = Vector4(corners[0].x, corners[0].y, corners[0].z, 1.0f);
	verts[1].pos = Vector4(corners[1].x, corners[1].y, corners[1].z, 1.0f);
	verts[2].pos = Vector4(corners[1].x, corners[1].y, corners[1].z, 1.0f);
	verts[3].pos = Vector4(corners[2].x, corners[2].y, corners[2].z, 1.0f);
	verts[4].pos = Vector4(corners[2].x, corners[2].y, corners[2].z, 1.0f);
	verts[5].pos = Vector4(corners[3].x, corners[3].y, corners[3].z, 1.0f);
	verts[6].pos = Vector4(corners[3].x, corners[3].y, corners[3].z, 1.0f);
	verts[7].pos = Vector4(corners[0].x, corners[0].y, corners[0].z, 1.0f);
	verts[8].pos = Vector4(corners[0].x, corners[0].y, corners[0].z, 1.0f);
	verts[9].pos = Vector4(corners[4].x, corners[4].y, corners[4].z, 1.0f);
	verts[10].pos = Vector4(corners[1].x, corners[1].y, corners[1].z, 1.0f);
	verts[11].pos = Vector4(corners[5].x, corners[5].y, corners[5].z, 1.0f);
	verts[12].pos = Vector4(corners[2].x, corners[2].y, corners[2].z, 1.0f);
	verts[13].pos = Vector4(corners[6].x, corners[6].y, corners[6].z, 1.0f);
	verts[14].pos = Vector4(corners[3].x, corners[3].y, corners[3].z, 1.0f);
	verts[15].pos = Vector4(corners[7].x, corners[7].y, corners[7].z, 1.0f);
	verts[16].pos = Vector4(corners[4].x, corners[4].y, corners[4].z, 1.0f);
	verts[17].pos = Vector4(corners[5].x, corners[5].y, corners[5].z, 1.0f);
	verts[18].pos = Vector4(corners[5].x, corners[5].y, corners[5].z, 1.0f);
	verts[19].pos = Vector4(corners[6].x, corners[6].y, corners[6].z, 1.0f);
	verts[20].pos = Vector4(corners[6].x, corners[6].y, corners[6].z, 1.0f);
	verts[21].pos = Vector4(corners[7].x, corners[7].y, corners[7].z, 1.0f);
	verts[22].pos = Vector4(corners[7].x, corners[7].y, corners[7].z, 1.0f);
	verts[23].pos = Vector4(corners[4].x, corners[4].y, corners[4].z, 1.0f);

	for (size_t j = 0; j < _countof(verts); ++j)
	{
		XMStoreFloat4(&verts[j].color, color);
	}

	s_DebugRendererPrimitives.insert(s_DebugRendererPrimitives.end(), verts, verts + 24);
}

void Hourglass::DebugRenderer::Render()
{
	// Make sure we've initialized render buffer
	assert(s_DebugRendererBuffer.HasVertexBuffer());

	if (s_NeedsUpdateBuffer)
	{
		int vertexCount = (int)s_DebugRendererPrimitives.size();
		int index = 0;

		// When vertex count exceeds vertex buffer size, use multiple draw calls
		while (vertexCount > 0)
		{
			s_DebugRendererBuffer.UpdateDynamicVertexBuffer(&s_DebugRendererPrimitives[index],
				sizeof(ColorVertex),
				min(vertexCount, s_VertexBufferSize));

			s_DebugRendererBuffer.Draw();

			index += s_VertexBufferSize;
			vertexCount -= s_VertexBufferSize;
		}

		s_NeedsUpdateBuffer = false;
	}
}

void Hourglass::DebugRenderer::Reset()
{
	s_DebugRendererPrimitives.clear();
	s_NeedsUpdateBuffer = true;
}

