#pragma once

class ActorShadow : public hg::IComponent
{
	DECLARE_COMPONENT_TYPEID
public:
	hg::IComponent* MakeCopyDerived() const;

	void Start();
	void Update();

private:
	hg::Transform*		m_Player;
	hg::MeshRenderer*	m_ShadowMesh;
};
