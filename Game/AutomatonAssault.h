#pragma once

#include "ImplodeExplode.h"
#include "BehaviorSet.h"
#include "ComponentSet.h"
#include "AssembledEntitySet.h"

class AutomatonAssault
{
public:

	void Init();

	void Start();

	static void CreateScene();

	void Shutdown();

private:

	void InitTestRobot(float x, float y, float z, float scale, float speed);
	
	void UnitTest_CollisionGroups();

	static BehaviorSet m_BehaviorSet;
	static ComponentSet m_ComponentSet;
	static AssembledEntitySet m_AssEntitySet;
};