#pragma once

#include "Clip.h"
#include "Component\Animation.h"

namespace Hourglass
{
	class AnimationManager
	{
	public:
		// Initialize mesh manager
		void Init();

		// Shutdown mesh manager
		void Shutdown();

		// Get a mesh by its path
		Clip* GetClip( const char* path );
		float GetLength( const char* path );
		LoadingSkeleton* GetSkeleton( const char* path );

	private:
		// Load meshes from 'Assets/Mesh' folder
		void LoadAnimations( const char* path );

		std::vector<Clip*>		m_Clips;
		std::vector<float>		m_Lengths;
		std::vector<LoadingSkeleton>	m_Skeletons;
	};

	extern AnimationManager g_AnimationManager;
}