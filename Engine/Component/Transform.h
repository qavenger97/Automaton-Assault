#pragma once
#include "..\Core\Math.h"
#include "..\Core\StrID.h"
#include <list>

namespace Hourglass
{
	class Entity;
	class Transform
	{
	public:
		struct TransformData
		{
			TransformData();
			XMFLOAT3 position;
			Quaternion rotation;
			XMFLOAT3 scale;
			static TransformData Identity();
		};

		void Init();

		void Shutdown();

	public:
		Transform(Entity* entity = nullptr);
		Entity* GetEntity()const;

		static Transform const& Identity();
		Transform* GetParent()const;
		uint32_t GetChildrenCount()const; 
		Transform* GetChild(uint32_t index)const; 
		Transform* FindChild(StrID name)const;
		void FindChildren(StrID name, std::vector<Transform*> &out)const;
		void Translate(const XMFLOAT3& offset);
		void Translate(const float x, const float y, const float z);
		void Scale(const XMFLOAT3& scale);
		void Scale(const float x, const float y, const float z);
		void Rotate(const Quaternion& rotation);
		void Rotate(const float pitch, const float yaw, const float roll);
		void SetPosition(const float x, const float y, const float z);
		void SetPosition(const XMFLOAT3& position);
		void SetScale(const XMFLOAT3& scale);
		void SetScale(const float x, const float y, const float z);
		void SetRotation(const Quaternion& rotation);
		void SetRotation(const float pitch, const float yaw, const float roll);
		void FlagChildren();
		void LocalRotation(const float pitch, const float yaw, const float roll);
		void LocalRotation(FXMVECTOR rotation);
		void Destroy();
		void Translate(FXMVECTOR translation);
		void Rotate(FXMVECTOR rotation);
		void Scale(FXMVECTOR scale);
		void SetPosition(FXMVECTOR position);
		void MoveTo(FXMVECTOR position, float dt);
		void SetRotation(FXMVECTOR rotation);
		void SetScale(FXMVECTOR scale);
		void RotateTo(FXMVECTOR direction, float dt);
		void UpdateTransform();
		void FixLocal();
		XMVECTOR Forward()const;
		XMVECTOR Up()const;
		XMVECTOR Right()const;

		// Build a rotation looking at target point
		static XMVECTOR BuildLookAtRotation(FXMVECTOR from, FXMVECTOR to, FXMVECTOR up = G_UP);
		static XMVECTOR BuildLookAtRotationFromDirectionVector(FXMVECTOR direction, FXMVECTOR up = G_UP);

		// Make transform facing a target point
		void LookAt(const FXMVECTOR& target);
		void TurnTo(const FXMVECTOR& target, float dt);
		/**
			Set parent to other transform
		*
		*/
		//void AddChild()
		void LinkTo(Transform* const other, bool keepWorld = true);
		void UnLink(bool alive = false);
		/**
			update children's transform
		*
		*/
		void UpdateChildren();
		/**
			Get matrix form of the transform
		*
		*/
		Transform& operator=(const Transform& rhs);
		XMVECTOR GetWorldPosition() const;
		XMVECTOR GetLocalPosition() const;
		XMVECTOR GetWorldRotation() const;
		XMVECTOR GetLocalRotation() const;
		XMVECTOR GetWorldScale() const;
		XMVECTOR GetLocalScale() const;

		XMMATRIX GetMatrix()const;
		/**
		Get inverse matrix form of the transform
		*
		*/
		XMMATRIX GetInverseMatrix()const;
		/**
			check if children need to be updated and update it;
		*
		*/
		void CheckAndUpdate();
		void Reset();
	protected:
		static Transform* root;
		Transform* m_parent;
		TransformData worldTransform;
		TransformData localTransform;
		std::list<Transform*> m_children;
	private:
		Entity* m_entity;
		bool m_bNeedUpdate;
	};
}
