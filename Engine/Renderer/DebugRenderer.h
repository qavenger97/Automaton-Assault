#pragma once

#include "../Math/Aabb.h"
#include "Collision/Cylinder.h"
#include "Component/Transform.h"

namespace Hourglass
{
	class DebugRenderer
	{
	public:
		// Initialize debug renderer
		static void Init();

		// Set line color for next primitive
		static void SetLineColor(const Vector4& color);

		// Get current line color
		static Vector4 GetLineColor();

		// Draw a line with current color
		static void DrawLine(const Vector3& v1, const Vector3& v2);

		// Draw a line with specified colors
		static void DrawLine(const Vector3& v1, const Vector3& v2, const Vector4& color);
		static void DrawLine(const Vector3& v1, const Vector3& v2, const Vector4& color1, const Vector4& color2);

		// Draw aabb with current color
		static void DrawAabb(const Aabb& aabb);

		// Draw aabb with a specified color
		static void DrawAabb(const Aabb& aabb, const Vector4& color);

		// Draw aabb with transform
		static void DrawAabb(const Aabb& aabb, const Matrix& m, const Vector4& color);

		static void DrawAxis(const Transform& transform);
		static void DrawAxis(const Matrix& m, float scale = 1.0f);

		// Draw sphere with current color
		static void DrawSphere(const Vector3& center, float radius, int segment = 12);

		// Draw sphere with specified color
		static void DrawSphere(const Vector3& center, float radius, const Vector4& color, int segment = 12);

		static void DrawCylinder(const Cylinder& cylinder, const Vector4& color, int segment = 12);

		static void DrawFrustum(const BoundingFrustum& frustum, const Color& color);

		// Send primitives to render API
		static void Render();

		// Clear debug render list for next frame
		static void Reset();
	};
}