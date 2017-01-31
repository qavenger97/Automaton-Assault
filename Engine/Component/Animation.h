#pragma once

#include "IComponent.h"
#include "..\Animation\Clip.h"
#include "Transform.h"

namespace Hourglass
{	
	struct Skeleton
	{
		static const uint32_t kMaxJoints = 96;
		Transform m_Transform[kMaxJoints];
		Matrix m_BindPoseInverse[kMaxJoints];
	};
	struct LoadingSkeleton
	{
		static const uint32_t kMaxJoints = 96;
		int8_t m_ParentIndices[kMaxJoints];
		Transform::TransformData m_GlobalJointPoses[kMaxJoints];
		Matrix m_BindPoseInverse[kMaxJoints];
	};	

	struct Pose
	{
		static const uint32_t kMaxJoints = 96;
		Matrix m_JointPoses[kMaxJoints];
	};

	struct BoneDown
	{
		uint32_t m_BoneId;
		uint32_t m_ClipId;
	};

	struct AnimationStates
	{
		enum
		{
			kLooping = (1 << 0),
			kHasChannel = (1 << 1)
		};

		static const uint32_t kMaxClips = 12;
		float m_CurrentTime[kMaxClips];
		float m_Length[kMaxClips];
		float m_Speed[kMaxClips];
		uint32_t m_Flags[kMaxClips];
	};

	class Animation : public IComponent
	{	
	public:		

		int32_t GetCurrentClipId() { return m_CurrentClipId; }

		void LoadFromXML( tinyxml2::XMLElement* data );

		bool IsPlaying();

		bool IsPlaying( StrID clipName );

		void Stop();

		unsigned int GetBoneCount() { return m_BoneCount; }

		void AddClip( Clip* clip, StrID clipName );

		void Play( StrID clipName, float speed, int boneDownId = 0 );

		void UpdateBoneDown();

		void Pause();

		void Resume();

		Skeleton* GetSkeleton() { return &m_Skeleton; }
		
		Transform& GetBoneTransform( unsigned int boneId ) { return m_Skeleton.m_Transform[boneId]; }

		void Update();

		void UpdateChannel( const Channel& channel, Transform& tween, float currentTime );

		void SetSkeleton( LoadingSkeleton* skeleton ) { m_LoadingSkeleton = skeleton; }

		LoadingSkeleton* GetLoadingSkeleton() const { return m_LoadingSkeleton; }

		void SetPose( Pose* pose ) { m_Pose = pose; }

		void SetWorldPose(Pose* pose) { m_WorldPose = pose; }

		const Matrix& GetBoneWorldPose(int boneId) const { return m_WorldPose->m_JointPoses[boneId]; }
		bool IsEnding()const;
		virtual void Shutdown() override;

		/**
		* Make a copy of this component
		*/
		virtual IComponent* MakeCopyDerived() const;

		virtual int GetTypeID() const { return s_TypeID; }

		bool IsPaused() { return m_Paused != 0; }

		static uint32_t s_TypeID;

	private:
		
		static const uint32_t kMaxJoints = 96;		
		static const uint32_t kMaxClips = 12;
		static const uint32_t kMaxBoneDowns = 64;

		Skeleton m_Skeleton;
		LoadingSkeleton* m_LoadingSkeleton = nullptr;
		Pose* m_Pose = nullptr;
		Pose* m_WorldPose = nullptr;
		AnimationStates m_AnimationStates;		
		Clip* m_AnimationClips[kMaxClips];
		uint32_t m_NumClips;
		int32_t m_CurrentClipId;
		uint32_t m_BoneCount;

		uint32_t m_IsAssembled : 1;
		uint32_t m_Paused : 1;

		BoneDown m_BoneDowns[kMaxBoneDowns];
		uint32_t m_BoneDownCount;
		
		std::unordered_map<StrID, uint32_t> m_MapFromClipNamesToIndices;
	};
}