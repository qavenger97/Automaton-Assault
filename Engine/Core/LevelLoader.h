#pragma once

namespace Hourglass
{
	class AISystem;

	class LevelLoader
	{
	public:

		void Init(AISystem* aiSys);

		void InitLevel(AISystem* aiSys);
	private:

		void LoadLevelXML( const char* path, AISystem* aiSys );

		void LoadStartLevelXML(AISystem* aiSys);

		void LoadAssembledXML();
	};
}