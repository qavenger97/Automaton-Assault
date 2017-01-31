#pragma once

class ChargerComp : public hg::IComponent
{
public:
	virtual int GetTypeID() const { return s_TypeID; }
	static uint32_t s_TypeID;

	void Init();

	void SetLunging( bool lunging ) { m_Lunging = lunging; }

	void OnCollision( Hourglass::Entity* other );

	hg::IComponent* MakeCopyDerived() const;

private:

	uint32_t m_Lunging : 1;
};