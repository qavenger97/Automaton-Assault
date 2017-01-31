#pragma once

namespace Hourglass 
{ 
	class DynamicCollider;
}

class ImplodeExplode : public Hourglass::IAction
{	
public:
	
	virtual void LoadFromXML( tinyxml2::XMLElement* data );

	virtual bool IsRunningChild() const override { return false; }

	void SetCircleSpeed( float circleSpeed ) { m_CircleSpeed = circleSpeed; }

	void SetTest( int test ) { m_Test = test; }

	void SetPlayer( hg::Transform* player ) { m_player = player; }

	virtual void Init( Hourglass::Entity* entity );

	virtual IBehavior::Result Update( Hourglass::Entity* entity );

	/**
	* Make a copy of this component
	*/
	virtual IBehavior* MakeCopy() const;

private:

	hg::Transform* m_player;
	float m_CircleSpeed;
	float m_Time;
	int m_Test;
	hg::DynamicCollider* m_dynColl;
};