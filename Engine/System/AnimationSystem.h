#pragma once

#include "Component\IComponent.h"
#include "..\Component\Animation.h"
#include "Core\ComponentPool.h"
#include "ISystem.h"

namespace Hourglass
{
	class AnimationSystem : public ISystem
	{
	public:

		void Init();

		const Pose* GetPoses() const { return m_Poses; }

		/*
		* Process all the animation for the current frame
		*/
		void Update();

		void FlattenPoses();

	private:

		static const unsigned int kMaxAnimations = 2048;
		LoadingSkeleton m_Skeletons[kMaxAnimations];
		Pose m_Poses[kMaxAnimations];
		Pose m_WorldPoses[kMaxAnimations];					// World transform of each joint
		Animation m_Animations[kMaxAnimations];
		ComponentPool<Animation> m_AnimationPool;
	};
}