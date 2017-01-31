#pragma once

#include "ISpatial.h"

namespace Hourglass
{
	template<typename T, Aabb(*FB)(const T*), bool(*FR)(const T*, const Ray&, float*, Vector3*, uint16_t)>
	class QuadtreeNode
	{
	public:
		QuadtreeNode(const Vector2& center, float size, int depth = 0)
		{
			for (int i = 0; i < 4; i++)
				children[i] = nullptr;

			this->size = size;
			this->center = center;
			this->depth = depth;
			nodeBound.pMin.x = center.x - size * 0.5f;
			nodeBound.pMin.z = center.y - size * 0.5f;
			nodeBound.pMax.x = center.x + size * 0.5f;
			nodeBound.pMax.z = center.y + size * 0.5f;
		}

		~QuadtreeNode()
		{
			for (int i = 0; i < 4; i++)
			{
				if (children[i])
					delete children[i];
			}
		}

		bool Insert(T* obj, const Aabb& aabb, int max_depth)
		{
			if (!Contains2D(nodeBound, aabb))
				return false;

			// Extent aabb in y direction
			if (nodeBound.pMin.y > aabb.pMin.y)
				nodeBound.pMin.y = aabb.pMin.y;

			if (nodeBound.pMax.y < aabb.pMax.y)
				nodeBound.pMax.y = aabb.pMax.y;

			if (depth < max_depth)
			{
				// Try inserting into child nodes
				if (aabb.pMax.x <= center.x && aabb.pMax.z <= center.y)
				{
					if (!children[0])
						children[0] = new QuadtreeNode((center + Vector2(nodeBound.pMin.x, nodeBound.pMin.z)) * 0.5f, size * 0.5f, depth + 1);
					children[0]->Insert(obj, aabb, max_depth);
					return true;
				}
				else if (aabb.pMax.x <= center.x && aabb.pMin.z >= center.y)
				{
					if (!children[1])
						children[1] = new QuadtreeNode((center + Vector2(nodeBound.pMin.x, nodeBound.pMax.z)) * 0.5f, size * 0.5f, depth + 1);
					children[1]->Insert(obj, aabb, max_depth);
					return true;
				}
				else if (aabb.pMin.x >= center.x && aabb.pMin.z >= center.y)
				{
					if (!children[2])
						children[2] = new QuadtreeNode((center + Vector2(nodeBound.pMax.x, nodeBound.pMax.z)) * 0.5f, size * 0.5f, depth + 1);
					children[2]->Insert(obj, aabb, max_depth);
					return true;
				}
				else if (aabb.pMin.x >= center.x && aabb.pMax.z <= center.y)
				{
					if (!children[3])
						children[3] = new QuadtreeNode((center + Vector2(nodeBound.pMax.x, nodeBound.pMin.z)) * 0.5f, size * 0.5f, depth + 1);
					children[3]->Insert(obj, aabb, max_depth);
					return true;
				}
			}

			objects.push_back(obj);
			return true;
		}

		bool RayCast(Ray& ray, T** outObj, float *t, Vector3* outNormal, uint16_t collisionGroupMasks)
		{
			if (!ray.Intersects(nodeBound) && !nodeBound.Contains(ray.Origin))
				return false;

			//DebugRenderer::DrawAabb(nodeBound, Color(0, 1, 0));

			bool result = false;

			// Test ray with objects in current node
			for (uint32_t i = 0; i < objects.size(); i++)
			{
				if (FR(objects[i], ray, t, outNormal, collisionGroupMasks))
				{
					ray.Distance = *t;
					result = true;
					*outObj = objects[i];
				}
			}

			// Test ray with objects in child nodes
			for (int i = 0; i < 4; i++)
			{
				if (children[i])
					result |= children[i]->RayCast(ray, outObj, t, outNormal, collisionGroupMasks);
			}

			return result;
		}

		void DebugDrawNode()
		{
			DebugRenderer::DrawAabb(nodeBound);

			for (int i = 0; i < 4; i++)
			{
				if (children[i])
					children[i]->DebugDrawNode();
			}
		}

		void DebugPrint(int depth = 0)
		{
			for (int i = 0; i < objects.size(); i++)
			{
				for (int j = 0; j < depth; j++)
				{
					OutputDebugStringA(" ");
				}

				OutputDebugStringA(objects[i]->GetEntity()->GetName().c_str());
				OutputDebugStringA("\n");
			}

			for (int i = 0; i < 4; i++)
			{
				if (children[i])
				{
					for (int j = 0; j < depth; j++)
					{
						OutputDebugStringA(" ");
					}

					char buf[1024];
					sprintf_s(buf, "Child %d (depth: %d)\n", i, depth + 1);
					OutputDebugStringA(buf);

					children[i]->DebugPrint(depth + 1);
				}
			}
		}

		QuadtreeNode*		children[4];
		Aabb				nodeBound;
		float				size;
		Vector2				center;			// XZ center of bound
		std::vector<T*>		objects;
		int					depth;

	private:

		// Return if a contains b in xz direction
		static bool Contains2D(const Aabb& a, const Aabb& b)
		{
			if (b.pMin.x < a.pMin.x || b.pMax.x > a.pMax.x)
				return false;

			if (b.pMin.z < a.pMin.z || b.pMax.z > a.pMax.z)
				return false;

			return true;
		}
	};

	template<typename T, Aabb(*FB)(const T*), bool(*FR)(const T*, const Ray&, float*, Vector3*, uint16_t)>
	class Quadtree : public ISpatial
	{
	public:
		Quadtree(int max_depth = 4)
			: m_Root(nullptr), m_MaxDepth(max_depth)
		{

		}

		~Quadtree()
		{
			delete m_Root;
		}

		void Init(const std::vector<T*>& objs)
		{
			Aabb treeBound;

			for (uint32_t i = 0; i < objs.size(); i++)
			{
				treeBound.Expand(FB(objs[i]));
			}

			Vector3 center = treeBound.GetCenter();
			float size_x = treeBound.pMax.x - treeBound.pMin.x;
			float size_z = treeBound.pMax.z - treeBound.pMin.z;

			float size = max(size_x, size_z) + 10.0f;

			assert(m_Root == nullptr);
			m_Root = new QuadtreeNode<T, FB, FR>(Vector2(center.x, center.z), size);
			//m_Root = new QuadtreeNode<T, FB, FR>(Vector2(0, 0), 200);

			for (uint32_t i = 0; i < objs.size(); i++)
			{
				m_Root->Insert(objs[i], FB(objs[i]), m_MaxDepth);
			}
		}

		void DebugRender()
		{
			m_Root->DebugDrawNode();
		}

		bool RayCast(const Ray& ray, T** outObj, float *t = nullptr, Vector3* outNormal = nullptr, uint16_t collisionGroupMasks = COLLISION_GROUP_ALL)
		{
			Ray testRay = ray;
			return m_Root->RayCast(testRay, outObj, t, outNormal, collisionGroupMasks);
		}

		void DebugPrint()
		{
			OutputDebugStringA("*** Begin Print Quadtree ***\n");
			m_Root->DebugPrint();
			OutputDebugStringA("*** End Print Quadtree ***\n");
		}

	private:
		QuadtreeNode<T, FB, FR>*	m_Root;
		int							m_MaxDepth;
	};

}