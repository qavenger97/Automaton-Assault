#include "pch.h"

#include "AnimationSystem.h"
#include "Core\ComponentRegister.h"
#include "Renderer\DebugRenderer.h"
#include "Core\Entity.h"

namespace Hourglass
{
	void AnimationSystem::Init()
	{
		ComponentPoolInit( SID(Animation), m_Animations, &m_AnimationPool, kMaxAnimations );

		for (unsigned int i = 0; i < kMaxAnimations; ++i)
		{
			m_Animations[i].SetPose( &m_Poses[i] );
			m_Animations[i].SetWorldPose( &m_WorldPoses[i] );
			m_Animations[i].SetSkeleton( &m_Skeletons[i] );
		}

	}

	void AnimationSystem::Update()
	{
		for (unsigned int animIt = 0; animIt < kMaxAnimations; ++animIt)
		{
			if (m_Animations[animIt].IsAlive() && m_Animations[animIt].IsEnabled())
			{				
				m_Animations[animIt].Update();				
			}			
		}		
	}
#if 0
	void AnimationSystem::FlattenPoses()
	{

		for (unsigned int animIt = 0; animIt < kMaxAnimations; ++animIt)
		{
			if (m_Animations[animIt].IsAlive() && m_Animations[animIt].IsEnabled())
			{

				//m_Animations[animIt].GetBoneTransform( 0 ).UpdateChildren();

				//m_Animations[animIt].Update();

				Skeleton& skeleton = *m_Animations[animIt].GetSkeleton();
				Pose& pose = m_Poses[animIt];

				//global = (skeleton.m_BindPoseInverse[ch] * skeleton.m_Transform[ch].GetMatrix());// *parent);

				Transform& local = skeleton.m_Transform[0];
				local.Rotate( 0.0f, 135.0f, 0.0f );

				//local.Rotate( 0.0f, 135.0f, 0.0f );
				const XMVECTOR& localPos = local.GetLocalPosition();
				Quaternion localRot = local.GetLocalRotation();
				const XMVECTOR& localScl = local.GetLocalScale();

				//Matrix local44 = XMMatrixAffineTransformation( Vector3( local.scale ), Quaternion::Identity, local.rotation, Vector3( local.position ) );

				Matrix local44 = XMMatrixAffineTransformation( Vector3( localScl ), Quaternion::Identity, localRot, Vector3( localPos) );

				pose.m_JointPoses[0] = (skeleton.m_BindPoseInverse[0] * local44);// *parent);
				//pose.m_JointPoses[0] *= m_Rot180;
				//Vector3 translation = rootGlobal.Translation();

				for (unsigned int ch = 1; ch < m_Animations[animIt].GetBoneCount(); ++ch)
				{
					Matrix& global = pose.m_JointPoses[ch];

					//global = (skeleton.m_BindPoseInverse[ch] * skeleton.m_Transform[ch].GetMatrix());// *parent);

					Transform& local = skeleton.m_Transform[ch];
					const XMVECTOR& localPos = local.GetLocalPosition();
					Quaternion localRot = local.GetLocalRotation();
					const XMVECTOR& localScl = local.GetLocalScale();

					Transform& parent = skeleton.m_Transform[m_Skeletons[animIt].m_ParentIndices[ch]];
					const XMVECTOR& parentPos = parent.GetLocalPosition();
					const Quaternion& parentRot = parent.GetLocalRotation();
					const XMVECTOR& parentScl = parent.GetLocalScale();

					//Matrix local44 = XMMatrixAffineTransformation( Vector3(local.scale), Quaternion::Identity, Quaternion( local.rotation ), Vector3( local.position ) );

					Vector3 final_pos_w_c = XMVector3Rotate( localPos * parentScl, parentRot ) + parentPos;
					Quaternion final_rot_w_c = XMQuaternionMultiply( localRot, parentRot );
					Vector3 final_scl_w_c = parentScl * localScl;

					local.SetPosition( final_pos_w_c );
					local.SetRotation( final_rot_w_c );
					local.SetScale( final_scl_w_c );

					if (parentRot.Dot( localRot ) < 0.0f)
					{
						localRot = -localRot;
					}

					//Matrix local44 = XMMatrixAffineTransformation( Vector3( local.scale ), Quaternion::Identity, local.rotation, Vector3( local.position ) );

					Matrix local44 = XMMatrixAffineTransformation( Vector3( final_scl_w_c ), Quaternion::Identity, final_rot_w_c, Vector3( final_pos_w_c ) );

					global = (skeleton.m_BindPoseInverse[ch] * local44);// *parent);
					//global *= m_Rot180;
				}				
			}
		}
	}
#else
	void AnimationSystem::FlattenPoses()
	{

		for (unsigned int animIt = 0; animIt < kMaxAnimations; ++animIt)
		{
			if (m_Animations[animIt].IsAlive() && m_Animations[animIt].IsEnabled() && !m_Animations[animIt].IsPaused())
			{
				//m_Animations[animIt].GetBoneTransform( 0 ).UpdateChildren();

				//m_Animations[animIt].Update();

				Skeleton& skeleton = *m_Animations[animIt].GetSkeleton();
				Pose& pose = m_Poses[animIt];
				Pose& worldPose = m_WorldPoses[animIt];

				//global = (skeleton.m_BindPoseInverse[ch] * skeleton.m_Transform[ch].GetMatrix());// *parent);

				Transform& local = skeleton.m_Transform[0];
				//local.Rotate(0.0f, XM_PI, 0.0f);
				//skeleton.m_Transform[1].Rotate(0.0f, XM_PI, 0.0f);
				const XMVECTOR& localPos = local.GetLocalPosition();
				XMVECTOR localRot = local.GetLocalRotation();
				const XMVECTOR& localScl = local.GetLocalScale();

				//Matrix local44 = XMMatrixAffineTransformation( Vector3( local.scale ), Quaternion::Identity, local.rotation, Vector3( local.position ) );

				XMMATRIX local44 = XMMatrixAffineTransformation(localScl, XMQuaternionIdentity(), localRot, localPos);

				pose.m_JointPoses[0] = XMMatrixMultiply(skeleton.m_BindPoseInverse[0], local44);// *parent);

				hg::Entity* ent = m_Animations[animIt].GetEntity();
				Vector3 entScale = ent->GetTransform()->GetWorldScale();
				Vector3 entPos = ent->GetTransform()->GetWorldPosition();

				/* // Root Debug
				worldPose.m_JointPoses[0] = local44;
				Matrix preBind = local44;
				DebugRenderer::DrawSphere( preBind.Translation()* entScale + entPos, 0.05f, Vector4(0.6f, 0.1f, 0.5f, 1.0f ));
				*/
																				 //Vector3 translation = rootGlobal.Translation();

				for (unsigned int ch = 1; ch < m_Animations[animIt].GetBoneCount(); ++ch)
				{
					Matrix& global = pose.m_JointPoses[ch];

					//global = (skeleton.m_BindPoseInverse[ch] * skeleton.m_Transform[ch].GetMatrix());// *parent);

					Transform& local = skeleton.m_Transform[ch];
					const XMVECTOR& localPos = local.GetLocalPosition();
					XMVECTOR localRot = local.GetLocalRotation();
					const XMVECTOR& localScl = local.GetLocalScale();

					Transform& parent = skeleton.m_Transform[m_Skeletons[animIt].m_ParentIndices[ch]];
					const XMVECTOR& parentPos = parent.GetLocalPosition();
					const XMVECTOR& parentRot = parent.GetLocalRotation();
					const XMVECTOR& parentScl = parent.GetLocalScale();

					//Matrix local44 = XMMatrixAffineTransformation( Vector3(local.scale), Quaternion::Identity, Quaternion( local.rotation ), Vector3( local.position ) );

					XMVECTOR final_pos_w_c = XMVector3Rotate(localPos * parentScl, parentRot) + parentPos;
					XMVECTOR final_rot_w_c = XMQuaternionMultiply(localRot, parentRot);
					final_rot_w_c = XMQuaternionNormalize(final_rot_w_c);
					XMVECTOR final_scl_w_c = parentScl * localScl;

					local.SetPosition(final_pos_w_c);
					local.SetRotation(final_rot_w_c);
					local.SetScale(final_scl_w_c);

					if (XMVectorGetX(XMVector3Dot(parentRot, localRot)) < 0.0f)
					{
						localRot = -(localRot);
					}

					//Matrix local44 = XMMatrixAffineTransformation( Vector3( local.scale ), Quaternion::Identity, local.rotation, Vector3( local.position ) );

					XMMATRIX local44 = XMMatrixAffineTransformation(final_scl_w_c, XMQuaternionIdentity(), final_rot_w_c, final_pos_w_c);

					worldPose.m_JointPoses[ch] = local44;
					global = XMMatrixMultiply(skeleton.m_BindPoseInverse[ch], local44);// *parent);

					/*  // Mesh Debug
					Matrix preBind = local44;					

					// Debug draw skeleton
					DebugRenderer::DrawLine( preBind.Translation() * entScale + entPos, entScale * worldPose.m_JointPoses[m_Skeletons[animIt].m_ParentIndices[ch]].Translation() + entPos );
					DebugRenderer::DrawSphere( preBind.Translation()* entScale + entPos, 0.05f );
					*/
				}
			}
		}
	}
#endif 
}