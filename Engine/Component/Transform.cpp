#include "pch.h"
#include "Transform.h"
#include "Core\Entity.h"
#include "Renderer\DevTextRenderer.h"
namespace Hourglass
{
	Transform* Transform::root = nullptr;

	Transform::Transform(Entity* entity)
		:
		m_parent(nullptr),
		m_bNeedUpdate(true),
		m_entity(entity)
	{
		worldTransform = localTransform = TransformData::Identity();
	}

	void Transform::Init()
	{
		worldTransform = localTransform = TransformData::Identity();
		m_parent = root;
		m_bNeedUpdate = true;
		//m_entity = nullptr;
	}

	void Transform::Shutdown()
	{
		root->m_children.remove(this);
	}

	Entity * Transform::GetEntity() const
	{
		return m_entity;
	}

	Transform const& Transform::Identity()
	{
		return *root;
	}
	Transform * Transform::GetParent() const
	{
		return m_parent;
	}
	uint32_t Transform::GetChildrenCount() const
	{
		return (uint32_t)m_children.size();
	}
	Transform* Transform::GetChild(uint32_t index) const
	{ 
		if (index >= m_children.size())return nullptr;
		auto itr = m_children.begin();
		for (uint32_t i = 0; i < index; i++)
		{
			itr++;
		}
		return *itr;
	}

	Transform* Transform::FindChild(StrID name) const
	{
		for (Transform* child : m_children)
		{
			if (child->GetEntity()->GetName() == name)
				return child;
		}
		return nullptr;
	}

	void Transform::FindChildren(StrID name, std::vector<Transform*>& out) const
	{
		for (Transform* child : m_children)
		{
			if (child->GetEntity()->GetName() == name)
				out.push_back(child);
		}
	}

	void Transform::Scale(const XMFLOAT3& scale)
	{
		XMVECTOR scl = XMLoadFloat3(&scale);
		Scale(scl);
	}

	void Transform::Scale(const float x, const float y, const float z)
	{
		Scale(XMVectorSet(x, y, z, 1));
	}

	void Transform::Rotate(const Quaternion & rotation)
	{
		XMVECTOR rot = XMLoadFloat4(&rotation);
		Rotate(rot);
	}

	void Transform::Rotate(const float pitch, const float yaw, const float roll)
	{
		XMVECTOR rot = XMQuaternionRotationRollPitchYaw(pitch, yaw, roll);
		Rotate(rot);
	}

	void Transform::SetPosition(const float x, const float y, const float z)
	{
		SetPosition(XMVectorSet(x,y,z,1));
	}

	void Transform::SetPosition(const XMFLOAT3 & position)
	{
		XMVECTOR pos = XMLoadFloat3(&position);
		SetPosition(pos);
	}

	void Transform::SetScale(const XMFLOAT3 & scale)
	{
		XMVECTOR scale_v = XMLoadFloat3(&scale);
		SetScale(scale_v);
	}

	void Transform::SetScale(const float x, const float y, const float z)
	{
		SetScale(XMVectorSet(x, y, z, 1));
	}

	void Transform::SetRotation(const Quaternion & rotation)
	{
		SetRotation(XMLoadFloat4(&rotation));
	}

	void Transform::SetRotation(const float pitch, const float yaw, const float roll)
	{
		SetRotation(XMQuaternionRotationRollPitchYaw(pitch, yaw, roll));
	}

	void Transform::FlagChildren()
	{
		for (Transform* child : m_children)
		{
			child->m_bNeedUpdate = true;
			child->FlagChildren();
		}
	}

	void Transform::LocalRotation(const float pitch, const float yaw, const float roll)
	{
		XMVECTOR rot = XMQuaternionRotationRollPitchYaw(pitch, yaw, roll);
		LocalRotation(rot);
	}

	void Transform::LocalRotation(FXMVECTOR rotation)
	{
		XMVECTOR rs = XMLoadFloat4(&localTransform.rotation);
		XMVECTOR rs_w = XMLoadFloat4(&worldTransform.rotation);
		rs = XMQuaternionMultiply(rs , rotation);
		rs_w = XMQuaternionMultiply(rs_w, rotation);
		rs = XMQuaternionNormalize(rs);
		rs_w = XMQuaternionNormalize(rs_w);
		XMStoreFloat4(&localTransform.rotation, rs);
		XMStoreFloat4(&worldTransform.rotation, rs_w);
	}

	void Transform::Destroy()
	{
		while(m_children.size() > 0)
		{
			m_children.front()->Destroy();
		}
		UnLink(false);
	}

	void Transform::Translate(const XMFLOAT3& offset)
	{
		XMVECTOR translation = XMLoadFloat3(&offset);
		Translate(translation);
	}

	void Transform::Translate(const float x, const float y, const float z)
	{
		Translate(XMVectorSet(x, y, z, 1));
	}

	Transform::TransformData::TransformData()
	{
		memset(&position, 0, 2 * sizeof XMFLOAT3);
		rotation.w = 1;
		scale = XMFLOAT3(1, 1, 1);
	}
	Transform::TransformData Transform::TransformData::Identity()
	{
		return TransformData();
	}

	void Transform::Translate(FXMVECTOR translation)
	{
		XMVECTOR rs_w = XMLoadFloat3(&worldTransform.position);
		rs_w += translation;
		XMStoreFloat3(&worldTransform.position, rs_w);
		XMVECTOR rs = XMLoadFloat3(&localTransform.position);
		rs += translation;
		XMStoreFloat3(&localTransform.position, rs);

		FlagChildren();
		m_bNeedUpdate = true;

	}

	void Transform::Rotate(FXMVECTOR rotation)
	{
		XMVECTOR rs = XMLoadFloat4(&localTransform.rotation);
		XMVECTOR rs_w = XMLoadFloat4(&worldTransform.rotation);
		rs = XMQuaternionMultiply(rotation, rs);
		rs_w = XMQuaternionMultiply(rotation, rs_w);
		rs = XMQuaternionNormalize(rs);
		rs_w = XMQuaternionNormalize(rs_w);
		XMStoreFloat4(&localTransform.rotation, rs);
		XMStoreFloat4(&worldTransform.rotation, rs_w);

		FlagChildren();
		m_bNeedUpdate = true;

	}

	void Transform::Scale(FXMVECTOR scale)
	{
		XMVECTOR rs_w = XMLoadFloat3(&worldTransform.scale);
		XMVECTOR rs = XMLoadFloat3(&localTransform.scale);
		rs = XMVectorMultiply(rs, scale);
		rs_w = XMVectorMultiply(rs_w, scale);
		XMStoreFloat3(&localTransform.scale, rs);
		XMStoreFloat3(&worldTransform.scale, rs_w);

		FlagChildren();
		m_bNeedUpdate = true;
		
	}

	void Transform::SetPosition(FXMVECTOR position)
	{
		XMVECTOR pos = XMLoadFloat3(&worldTransform.position);
		XMVECTOR offset = position - pos;
		Translate(offset);
	}

	void Transform::MoveTo(FXMVECTOR position, float dt)
	{
		XMVECTOR finalPos = XMVectorLerp(GetWorldPosition(), position, dt);
		SetPosition(finalPos);
	}

	void Transform::SetRotation(FXMVECTOR rotation)
	{
		XMVECTOR rot_world = XMLoadFloat4(&worldTransform.rotation);
		XMVECTOR rot = XMQuaternionMultiply( rotation, XMQuaternionConjugate(rot_world));
		Rotate(rot);
	}

	void Transform::SetScale(FXMVECTOR scale)
	{
		XMVECTOR scale_world = XMLoadFloat3(&worldTransform.scale);
		XMVECTOR scale_local = XMLoadFloat3(&localTransform.scale);
		XMVECTOR ratio = scale / scale_world;
		scale_local *= ratio;
		Scale(ratio);
	}

	void Transform::RotateTo(FXMVECTOR direction, float dt)
	{
		XMVECTOR Rot = XMLoadFloat4(&worldTransform.rotation);
		XMVECTOR finalRot = XMQuaternionSlerp(Rot, direction, dt);
		SetRotation(finalRot);
	}

	void Transform::UpdateTransform()
	{
		XMStoreFloat3(&worldTransform.position, GetParent()->GetWorldPosition());
		XMStoreFloat4(&worldTransform.rotation, GetParent()->GetWorldRotation());
		XMStoreFloat3(&worldTransform.scale , GetParent()->GetWorldScale() );
		localTransform = TransformData();
	}

	void Transform::FixLocal()
	{
		m_bNeedUpdate = false;
	}

	XMVECTOR Transform::Forward() const
	{
		XMVECTOR forward = XMLoadFloat4(&worldTransform.rotation);
		forward = XMVector3Rotate(G_FORWARD, forward);
		return forward;
	}

	XMVECTOR Transform::Up() const
	{
		XMVECTOR up = XMLoadFloat4(&worldTransform.rotation);
		up = XMVector3Rotate(G_UP, up);
		return up;;
	}

	XMVECTOR Transform::Right() const
	{
		XMVECTOR right = XMLoadFloat4(&worldTransform.rotation);
		right = XMVector3Rotate(G_RIGHT, right);
		return right;;
	}
	XMVECTOR Transform::BuildLookAtRotationFromDirectionVector(FXMVECTOR direction, FXMVECTOR up)
	{
		XMVECTOR forward = XMVector3Normalize(direction);
		if (XMVectorGetX(XMVectorEqual(forward, XMVectorZero())))
		{
			return XMQuaternionIdentity();
		}
		XMVECTOR right = XMVector3Normalize(XMVector3Cross(up, forward));
		XMVECTOR local_up = XMVector3Normalize(XMVector3Cross(forward, right));
		XMMATRIX mat = XMMATRIX(right, local_up, forward, XMQuaternionIdentity());

		return XMQuaternionNormalize(XMQuaternionRotationMatrix(mat));
	}

	XMVECTOR Transform::BuildLookAtRotation(FXMVECTOR from, FXMVECTOR to, FXMVECTOR up)
	{
		return BuildLookAtRotationFromDirectionVector(to - from);
	}

	void Transform::LookAt(const FXMVECTOR& target)
	{
		SetRotation(BuildLookAtRotation(GetWorldPosition(), target) );
	}

	void Transform::TurnTo(const FXMVECTOR& target, float dt)
	{
		XMVECTOR newRot = BuildLookAtRotation(GetWorldPosition(), target);
		
		XMVECTOR Rot = XMLoadFloat4(&worldTransform.rotation);
		XMVECTOR finalRot = XMQuaternionSlerp(Rot, newRot, dt);

		SetRotation(finalRot);
	}

	void Transform::UpdateChildren()
	{
		XMVECTOR world_pos = XMLoadFloat3(&worldTransform.position);
		XMVECTOR world_rot = XMLoadFloat4(&worldTransform.rotation);
		XMVECTOR world_scl = XMLoadFloat3(&worldTransform.scale);
		for (Transform* child : m_children)
		{
			if (child->m_bNeedUpdate)
			{
				XMVECTOR local_pos_c = XMLoadFloat3(&child->localTransform.position);
				XMVECTOR local_rot_c = XMLoadFloat4(&child->localTransform.rotation);
				XMVECTOR local_scl_c = XMLoadFloat3(&child->localTransform.scale);

				XMVECTOR final_pos_w_c = XMVector3Rotate(local_pos_c * world_scl, world_rot) + world_pos;
				XMVECTOR final_rot_w_c = XMQuaternionMultiply( local_rot_c, world_rot);
				XMVECTOR final_scl_w_c = world_scl * local_scl_c;
				final_rot_w_c = XMQuaternionNormalize(final_rot_w_c);
				XMStoreFloat3(&child->worldTransform.position, final_pos_w_c);
				XMStoreFloat4(&child->worldTransform.rotation, final_rot_w_c);
				XMStoreFloat3(&child->worldTransform.scale, final_scl_w_c);
			}
			child->UpdateChildren();
		}
		m_bNeedUpdate = false;
	}

	Transform & Transform::operator=(const Transform & rhs)
	{
		worldTransform = rhs.worldTransform;
		localTransform = rhs.localTransform;
		return *this;
	}

	XMVECTOR Transform::GetWorldPosition() const
	{
		return XMLoadFloat3(&worldTransform.position);
	}

	XMVECTOR Transform::GetLocalPosition() const
	{
		return XMLoadFloat3(&localTransform.position);
	}

	XMVECTOR Transform::GetWorldRotation() const
	{
		return XMLoadFloat4(&worldTransform.rotation);
	}

	XMVECTOR Transform::GetLocalRotation() const
	{
		return XMLoadFloat4(&localTransform.rotation);
	}

	XMVECTOR Transform::GetWorldScale() const
	{
		return XMLoadFloat3(&worldTransform.scale);
	}

	XMVECTOR Transform::GetLocalScale() const
	{
		return XMLoadFloat3(&localTransform.scale);
	}

	void Transform::LinkTo(Transform* const other, bool keepWorld)
	{
		m_parent->m_children.remove(this);
		m_parent = other;
		other->m_children.push_back(this);
		if (keepWorld)
		{
			XMVECTOR world_pos_other = XMLoadFloat3(&other->worldTransform.position);
			XMVECTOR world_rot_other = XMLoadFloat4(&other->worldTransform.rotation);
			XMVECTOR world_scl_other = XMLoadFloat3(&other->worldTransform.scale);
			XMVECTOR world_pos_this = XMLoadFloat3(&worldTransform.position);
			XMVECTOR world_rot_this = XMLoadFloat4(&worldTransform.rotation);
			XMVECTOR world_scl_this = XMLoadFloat3(&worldTransform.scale);

			XMVECTOR world_rot_other_inv = XMQuaternionConjugate(world_rot_other);

			XMVECTOR final_local_pos = XMVector3Rotate((world_pos_this - world_pos_other), world_rot_other_inv) / world_scl_other;
			XMStoreFloat3(&localTransform.position, final_local_pos);

			XMVECTOR final_local_scl = world_scl_this / world_scl_other;
			XMStoreFloat3(&localTransform.scale, final_local_scl);

			XMVECTOR final_local_rot = XMQuaternionMultiply(world_rot_this, world_rot_other_inv);
			final_local_rot = XMQuaternionNormalize(final_local_rot);
			XMStoreFloat4(&localTransform.rotation, final_local_rot);
		}
	}
	void Transform::UnLink(bool alive)
	{
		if (alive)
		{
			LinkTo( root );
			return;
		}

		if (m_parent)
		{
			m_parent->m_children.remove( this );
			m_parent = nullptr;
		}
		
	}
	void Transform::CheckAndUpdate()
	{
		UpdateChildren();	
	}

	void Transform::Reset()
	{
		LinkTo(root);
	}

	XMMATRIX Transform::GetMatrix()const
	{
		/*
		// Writes the transform data in text at the world position
		char buf[1024];
		//sprintf_s( buf, "\n\nPos: %f,%f,%f", pos.x, pos.y, pos.z );
		sprintf_s( buf, "\n\nPos: %f,%f,%f \nRot: %f,%f,%f,%f \nScale: %f,%f,%f", 
			worldTransform.position.x, worldTransform.position.y, worldTransform.position.z, 
			worldTransform.rotation.x, worldTransform.rotation.y, worldTransform.rotation.z, worldTransform.rotation.w, 
			worldTransform.scale.x, worldTransform.scale.y, worldTransform.scale.z );
		DevTextRenderer::DrawText_WorldSpace( buf, XMLoadFloat3(&worldTransform.position) );
		*/
		return XMMatrixAffineTransformation(XMLoadFloat3(&worldTransform.scale), XMQuaternionIdentity(), XMLoadFloat4(&worldTransform.rotation), XMLoadFloat3(&worldTransform.position));
	}

	XMMATRIX Transform::GetInverseMatrix() const
	{
		XMVECTOR inv_pos = -XMLoadFloat3(&worldTransform.position);
		XMVECTOR inv_rot = XMLoadFloat4(&worldTransform.rotation);
		XMVECTOR inv_scl = XMLoadFloat3(&worldTransform.scale);
		XMVECTOR one = XMVectorSet(1, 1, 1, 1);
		inv_rot = XMQuaternionConjugate(inv_rot);
		inv_pos = XMVector3Rotate(inv_pos, inv_rot);
		inv_scl = one / inv_scl;
		
		return XMMatrixAffineTransformation(inv_scl, XMQuaternionIdentity(), inv_rot, inv_pos);
	}
}