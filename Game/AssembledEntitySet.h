#pragma once

class AssembledEntitySet
{
public:

	void Init();
	void LoadSceneEntities();
	void Shutdown();

private:

	//hg::Entity* InitPlayer();
	void InitTestRobotAssembler();
	void InitBulletAssembler();

	void CleanupTestRobotAssembler();
	void CleanupBulletAssembler();
private:
	hg::PrimitiveRenderer* pr;
	hg::Entity* m_TestBot;
	hg::Entity* m_bullet;
};