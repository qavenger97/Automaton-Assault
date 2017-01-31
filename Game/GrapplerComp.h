#pragma once

class GrapplerComp : public hg::IComponent
{
public:
	virtual int GetTypeID() const { return s_TypeID; }
	static uint32_t s_TypeID;

	void LoadFromXML( tinyxml2::XMLElement* data );

	void SetGrapple( hg::Entity* grapple );

	void SetGrappleStartPos( const Vector3& startpos ) { m_GrappleStartPos; }

	const Vector3& GetGrappleStartPos() const { return m_GrappleStartPos; }

	void SetRadialPatrolStartPoint( const Vector3& vector ) { m_RadialPatrolStartPoint = vector; }
	const Vector3& GetRadialPatrolStartPoint() { return m_RadialPatrolStartPoint; }

	virtual void Shutdown() override;

	void OnMessage(hg::Message* msg);

	hg::IComponent* MakeCopyDerived() const;

private:

	Vector3 m_RadialPatrolStartPoint;
	hg::Entity* m_Grapple;
	Vector3 m_GrappleStartPos;
};