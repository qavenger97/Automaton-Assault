#include "pch.h"

#include "Animation.h"
#include "..\System\AnimationSystem.h"
#include "Core\ComponentFactory.h"
#include "Core\FileArchive.h"
#include "Animation\AnimationManager.h"
#include "Renderer\DebugRenderer.h"
#include "Core\Entity.h"
#include "Core\TimeManager.h"
#include "Renderer\DevTextRenderer.h"
#include "Core\Entity.h"

namespace Hourglass
{
	uint32_t Animation::s_TypeID = ComponentFactory::GetSystemComponentID();

	void Animation::LoadFromXML( tinyxml2::XMLElement* data )
	{
		m_Paused = false;
		m_BoneDownCount = 0;
		m_NumClips = 0;
		m_CurrentClipId = 0;

		tinyxml2::XMLElement* clipSetXML = data->FirstChildElement( "Clips" );
		tinyxml2::XMLElement* clipXML = clipSetXML->FirstChildElement( "Clip" );

		memset( &m_AnimationStates, 0, sizeof( AnimationStates ) );

		bool linked = false;

		while (clipXML)
		{
			bool setAsStart = false;
			clipXML->QueryBoolAttribute( "setAsStart", &setAsStart );

			if (setAsStart)
			{
				m_CurrentClipId = m_NumClips;
			}

			bool isLooping = false;
			clipXML->QueryBoolAttribute( "looping", &isLooping );

			m_AnimationStates.m_Flags[m_NumClips] = 0;

			if (isLooping)
			{
				m_AnimationStates.m_Flags[m_NumClips] |= AnimationStates::kLooping;
			}			

			const char* clipFile = clipXML->Attribute( "file" );
			m_AnimationStates.m_CurrentTime[m_NumClips] = 0.0f;
			m_AnimationStates.m_Speed[m_NumClips] = 1.0f;
			m_AnimationStates.m_Length[m_NumClips] = g_AnimationManager.GetLength( clipFile );
			m_AnimationClips[m_NumClips] = g_AnimationManager.GetClip( clipFile );

			if (!linked)
			{
				LoadingSkeleton* skeleton = g_AnimationManager.GetSkeleton( clipFile );

				unsigned int currTransform = 1;

				m_Skeleton.m_Transform[0].Init();
				m_Skeleton.m_BindPoseInverse[0] = skeleton->m_BindPoseInverse[0];

				while (skeleton->m_ParentIndices[currTransform] > -1)
				{
					m_Skeleton.m_Transform[currTransform].Init();
					m_Skeleton.m_Transform[currTransform].LinkTo( &m_Skeleton.m_Transform[skeleton->m_ParentIndices[currTransform]] );
					m_Skeleton.m_BindPoseInverse[currTransform] = skeleton->m_BindPoseInverse[currTransform];
					++currTransform;
				}

				m_BoneCount = currTransform - 1;

				m_IsAssembled = false;

				if (skeleton)
				{
					if (m_LoadingSkeleton == nullptr)
					{
						m_IsAssembled = true;
						m_LoadingSkeleton = new LoadingSkeleton;
					}

					*m_LoadingSkeleton = *skeleton;
				}

				linked = true;
			}

			const char* clipName = clipXML->Attribute( "name" );
			StrID clipStr = WSID( clipName );
			m_MapFromClipNamesToIndices[clipStr] = m_NumClips;

			++m_NumClips;

			clipXML = clipXML->NextSiblingElement();
		}

		//if(m_Pose)
		//	m_Pose->m_JointPoses[0] = Matrix::Identity;
	}

	void Animation::AddClip( Clip * clip, StrID clipName )
	{
		m_MapFromClipNamesToIndices[clipName] = m_NumClips++;
	}

	void Animation::Play( StrID clipName, float speed, int boneDownId )
	{
		unsigned int clipId = m_MapFromClipNamesToIndices[clipName];
		
		m_AnimationStates.m_CurrentTime[clipId] = 0.0f;
		m_AnimationStates.m_Speed[clipId] = speed;		

		if (boneDownId > 0)
		{					
			bool boneExists = false;

			for (unsigned int i = 0; i < m_BoneDownCount; ++i)
			{
				if (m_BoneDowns[i].m_BoneId == boneDownId)
				{
					boneExists = true;

					unsigned int boneDownClipId = m_BoneDowns[i].m_ClipId;
					if (m_BoneDowns[i].m_ClipId != clipId)
					{
						m_AnimationStates.m_CurrentTime[boneDownClipId] = m_AnimationStates.m_Length[boneDownClipId] + 1.0f;
						m_BoneDowns[i].m_ClipId = clipId;
						m_AnimationStates.m_CurrentTime[clipId] = 0.0f;
					}
					
					m_AnimationStates.m_Flags[clipId] |= AnimationStates::kHasChannel;
				}
			}

			if (!boneExists)
			{
				m_BoneDowns[m_BoneDownCount].m_ClipId = clipId;
				assert( clipId != 0 );
				m_BoneDowns[m_BoneDownCount++].m_BoneId = boneDownId;
			}
			
		}
		else
		{
			m_AnimationStates.m_CurrentTime[m_CurrentClipId] = 0.0f;
			m_CurrentClipId = clipId;
			m_AnimationStates.m_Flags[clipId] &= ~(AnimationStates::kHasChannel);
		}		
	}

	void Animation::UpdateBoneDown()
	{
		unsigned int endlessCheck = 0;

		for (unsigned int boneDownIt = 0; boneDownIt < m_BoneDownCount; ++boneDownIt)
		{
			unsigned int currbone = m_BoneDowns[boneDownIt].m_BoneId;
			unsigned int currClip = m_BoneDowns[boneDownIt].m_ClipId;			

			unsigned int currId = currbone + 1;
			const Channel* channels = m_AnimationClips[currClip]->GetChannels();

			float& currentTime = m_AnimationStates.m_CurrentTime[currClip];
			float speed = m_AnimationStates.m_Speed[currClip];
			float length = m_AnimationStates.m_Length[currClip];

			bool isLooping = m_AnimationStates.m_Flags[currClip] & AnimationStates::kLooping;

			// reset the currentTime if it has surpassed the length
			if (isLooping)
			{
				while (currentTime > length)
				{					
					currentTime -= length;
				}
			}
			else if (currentTime > length)
			{
				m_AnimationStates.m_Flags[currClip] &= ~(AnimationStates::kHasChannel);
				continue;
			}

			if (currClip != m_CurrentClipId)
				currentTime += g_Time.Delta() * speed;

			const Channel& channel = channels[currbone];
			UpdateChannel( channel, m_Skeleton.m_Transform[currbone], currentTime );			

			for (; currId < kMaxJoints; ++currId)
			{
				int currParentId = m_LoadingSkeleton->m_ParentIndices[currId];
				while (currParentId != currbone && currParentId != UCHAR_MAX && currParentId > 0 && endlessCheck < 9001)
				{
					++endlessCheck;

					currParentId = m_LoadingSkeleton->m_ParentIndices[currParentId];
				}

				if (currParentId != currbone) 
				{
					continue; // not part of this bone down animation
				}				

				const Channel& channel = channels[currId];
				UpdateChannel( channel, m_Skeleton.m_Transform[currId], currentTime );
			}
		}

		assert( endlessCheck < 9001 );
	}

	void Animation::Pause()
	{
		m_Paused = true;
	}

	void Animation::Resume()
	{
		m_Paused = false;
	}

	void Animation::Stop()
	{
		m_AnimationStates.m_CurrentTime[m_CurrentClipId] = 0.0f;
		m_CurrentClipId = 0;
	}

	bool Animation::IsPlaying()
	{
		return (m_CurrentClipId >= 0) ||
			(m_AnimationStates.m_Flags[m_CurrentClipId] & AnimationStates::kLooping);
	}

	bool Animation::IsPlaying( StrID clipName )
	{
		unsigned int clipId = m_MapFromClipNamesToIndices[clipName];

		float currTime = m_AnimationStates.m_CurrentTime[clipId];
		return (currTime != 0.0f && currTime < m_AnimationStates.m_Length[clipId]);
	}

	void Animation::Update()
	{
		if (m_Paused)
			return;

		if (m_CurrentClipId < 0)
		{
			m_CurrentClipId = 0;
			m_AnimationStates.m_CurrentTime[0] = 0.0f;
		}
		
		// get the animation state
		float& currentTime = m_AnimationStates.m_CurrentTime[m_CurrentClipId];
		float speed = m_AnimationStates.m_Speed[m_CurrentClipId];
		float length = m_AnimationStates.m_Length[m_CurrentClipId];

		/**/
		if (m_AnimationStates.m_Flags[m_CurrentClipId] & AnimationStates::kHasChannel)
		{
			UpdateBoneDown();
			return;
		}

		bool isLooping = m_AnimationStates.m_Flags[m_CurrentClipId] & AnimationStates::kLooping;

		currentTime += g_Time.Delta() * speed;
		// reset the currentTime if it has surpassed the length
		if (isLooping)
		{
			while (currentTime > length)
			{
				currentTime -= length;
			}
		}
		else if (currentTime > length)
		{
			m_AnimationStates.m_Flags[m_CurrentClipId] &= ~(AnimationStates::kHasChannel);
			m_CurrentClipId = 0;
			m_AnimationStates.m_CurrentTime[0] = 0.0f;
			currentTime = m_AnimationStates.m_CurrentTime[0];
			speed = m_AnimationStates.m_Speed[0];
			length = m_AnimationStates.m_Length[0];
		}

		// get the channels
		const Channel* channels = m_AnimationClips[m_CurrentClipId]->GetChannels();
		unsigned int channelCount = m_AnimationClips[m_CurrentClipId]->GetChannelCount();

		// handle the case that the current time is 0, setting the keyframe data to the inital frame
		if (currentTime == 0.0f)
		{
			for (unsigned int i = 0; i < channelCount; ++i)
			{
				const Channel &channel = channels[i];
				const KeyFrame* keyFrames = channel.GetChannelKeyFrames();
				if (keyFrames != nullptr)
				{
					m_Skeleton.m_Transform[i].SetPosition( keyFrames[1].m_Local.position );
					m_Skeleton.m_Transform[i].SetRotation( keyFrames[1].m_Local.rotation );
					m_Skeleton.m_Transform[i].SetScale( keyFrames[1].m_Local.scale );
				}
			}

			return;
		}

		const Channel &channel = channels[0];
		unsigned int keyCount = channel.GetChannelCount();
		const float* keyTimes = channel.GetChannelKeyTimes();
		const KeyFrame* keyFrames = channel.GetChannelKeyFrames();

		for (unsigned int kf = 2; kf < keyCount; ++kf)
		{
			if (currentTime < keyTimes[kf])
			{
				const KeyFrame& front = keyFrames[kf];
				const KeyFrame& back = keyFrames[kf - 1];

				Transform &tween = m_Skeleton.m_Transform[0];
					
				float frontTime = keyTimes[kf];
				float backTime = keyTimes[kf - 1];

				float frameTime = max(0,currentTime - backTime);
				float tweenTime = frontTime - backTime;
				float dt = frameTime / tweenTime;

				tween.SetRotation(Quaternion::Slerp( back.m_Local.rotation, front.m_Local.rotation, dt ));
				tween.SetPosition(Vector3::Lerp( back.m_Local.position, front.m_Local.position, dt ));
				tween.SetScale(Vector3::Lerp( back.m_Local.scale, front.m_Local.scale, dt ));

				/*
				tween.rotation = Quaternion::Lerp( back.m_Local.rotation, front.m_Local.rotation, dt );
				tween.position = Vector3::Lerp( back.m_Local.position, front.m_Local.position, dt );
				tween.scale = Vector3::Lerp( back.m_Local.scale, front.m_Local.scale, dt );
				*/

				break;
			}
		}

		// set the keyframe data for each channel at the current frame
		for (unsigned int ch = 1; ch < channelCount; ++ch)
		{
			const Channel &channel = channels[ch];

			UpdateChannel( channel, m_Skeleton.m_Transform[ch], currentTime );
		}

		UpdateBoneDown();
	}

	void Animation::UpdateChannel( const Channel& channel, Transform& tween, float currentTime )
	{
		unsigned int keyCount = channel.GetChannelCount();
		const float* keyTimes = channel.GetChannelKeyTimes();
		const KeyFrame* keyFrames = channel.GetChannelKeyFrames();

		for (unsigned int kf = 2; kf < keyCount; ++kf)
		{
			if (currentTime < keyTimes[kf])
			{
				//char buf[1024];
				//sprintf_s(buf, "Key Time: %.6f", keyTimes[kf]);
				//DevTextRenderer::DrawText_WorldSpace( buf, GetEntity()->GetTransform()->GetWorldPosition() );

				const KeyFrame& front = keyFrames[kf];
				const KeyFrame& back = keyFrames[kf - 1];				

				float frontTime = keyTimes[kf];
				float backTime = keyTimes[kf - 1];

				float frameTime = currentTime - backTime;
				float tweenTime = frontTime - backTime;
				float dt = frameTime / tweenTime;

				tween.SetRotation( Quaternion::Lerp( back.m_Local.rotation, front.m_Local.rotation, dt ) );
				tween.SetPosition( Vector3::Lerp( back.m_Local.position, front.m_Local.position, dt ) );
				tween.SetScale( Vector3::Lerp( back.m_Local.scale, front.m_Local.scale, dt ) );

				break;
			}
		}
	}

	bool Animation::IsEnding() const
	{
		return fabsf(m_AnimationStates.m_CurrentTime[m_CurrentClipId] - m_AnimationStates.m_Length[m_CurrentClipId]) <= hg::g_Time.Delta() * 2;
	}

	void Animation::Shutdown()
	{
		IComponent::Shutdown();
		m_Skeleton.m_Transform[0].Destroy();
		if (m_IsAssembled)
			delete m_LoadingSkeleton;
	}

	IComponent* Animation::MakeCopyDerived() const
	{
		Animation* cpy = (Animation*)IComponent::Create( SID( Animation ) );
		for (unsigned int i = 0; i < m_NumClips; ++i)
		{
			cpy->m_AnimationClips[i] = m_AnimationClips[i];
		}

		cpy->m_NumClips = m_NumClips;
		cpy->m_MapFromClipNamesToIndices = m_MapFromClipNamesToIndices;

		*cpy->m_LoadingSkeleton = *m_LoadingSkeleton;
		unsigned int currTransform = 1;
		
		cpy->m_Skeleton.m_Transform[0].Init();
		cpy->m_Skeleton.m_BindPoseInverse[0] = m_LoadingSkeleton->m_BindPoseInverse[0];

		while (m_LoadingSkeleton->m_ParentIndices[currTransform] > -1)
		{
			cpy->m_Skeleton.m_Transform[currTransform].Init();
			cpy->m_Skeleton.m_Transform[currTransform].LinkTo( &cpy->m_Skeleton.m_Transform[m_LoadingSkeleton->m_ParentIndices[currTransform]] );
			cpy->m_Skeleton.m_BindPoseInverse[currTransform] = m_LoadingSkeleton->m_BindPoseInverse[currTransform];
			++currTransform;
		}

		cpy->m_AnimationStates = m_AnimationStates;

		for (unsigned int i = 0; i < kMaxClips; ++i)
		{
			cpy->m_AnimationClips[i] = m_AnimationClips[i];
		}

		for (unsigned int i = 0; i < kMaxBoneDowns; ++i)
		{
			cpy->m_BoneDowns[i] = m_BoneDowns[i];
		}
		
		cpy->m_CurrentClipId = m_CurrentClipId;
		cpy->m_BoneCount = m_BoneCount;
		
		cpy->m_BoneDownCount = m_BoneDownCount;
		cpy->m_Paused = m_Paused;
		return cpy;
	}
}
