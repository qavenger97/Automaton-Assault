#include "GamePch.h"

#include "AI_CarrierReleaseEnemies.h"

void AI_CarrierReleaseEnemies::LoadFromXML( tinyxml2::XMLElement * data )
{
	m_Duration = 2.8f;
	data->QueryFloatAttribute( "duration", &m_Duration );
}

void AI_CarrierReleaseEnemies::Init( hg::Entity * entity )
{
	m_Carrier = hg::Entity::FindByTag( SID(RadialCarrier) )->GetTransform();

	int childCount = m_Carrier->GetChildrenCount();
	
	int currEnemy = 0;

	for (int i = 0; i < childCount; ++i)
	{
		hg::Entity* child = m_Carrier->GetChild( i )->GetEntity();
		hg::Motor* motor = child->GetComponent<hg::Motor>();
		if (motor)
		{
			m_Enemies[currEnemy++] = child;
			
			hg::Transform* childtf = child->GetTransform();
			childtf->UnLink(true);
			
			// Remove non-yaw rotation
			Quaternion rot = childtf->GetWorldRotation();
			rot.y = 0;
			rot.z = 0;
			rot.Normalize();
			childtf->SetRotation( rot );
			
			Vector3 pos = childtf->GetWorldPosition();
			pos.y = 0.5f;
			childtf->SetPosition( pos );

			i = -1; // HACK: this algorithm isn't in linear time like it could be, but it will work for now
			childCount = m_Carrier->GetChildrenCount();
		}
	}	

	// sort all by z value
	int asize = kRows * kCols;
	for (int outer = 0; outer < asize; ++outer)
	{

		int min = outer;

		for (int inner = outer; inner < asize; inner++)
		{
			if (m_Enemies[min]->GetPosition().z > m_Enemies[inner]->GetPosition().z)
				min = inner;
		}

		hg::Entity* temp = m_Enemies[outer];
		m_Enemies[outer] = m_Enemies[min];
		m_Enemies[min] = temp;
	}

	// sort each row by x value
	for (int row = 0; row < kRows; ++row)
	{
		for (int outer = 0; outer < kCols; ++outer)
		{
			int offset = row * kCols;
			int min = outer;

			for (int inner = outer; inner < kCols; inner++)
				if (m_Enemies[offset + min]->GetPosition().z > m_Enemies[offset + inner]->GetPosition().z)
					min = inner;

			hg::Entity* temp = m_Enemies[offset + outer];
			m_Enemies[offset + outer] = m_Enemies[offset + min];
			m_Enemies[offset + min] = temp;
		}
	}	

	m_Timer = m_Duration;
	m_CurrRow = 0;
}

Hourglass::IBehavior::Result AI_CarrierReleaseEnemies::Update( Hourglass::Entity* entity )
{
	m_Timer += hg::g_Time.Delta();

	if (m_CurrRow == kRows)
		if (m_Timer >= m_Duration)
			return IBehavior::kSUCCESS;
	
	if (m_Timer >= m_Duration)
	{
		for (int i = 0; i < kCols; ++i)
		{
			hg::Entity* enemy = m_Enemies[m_CurrRow * kCols + i];
			Quaternion rot = Quaternion::CreateFromYawPitchRoll( .33f - i * .22f, 0.0f, 0.0f );
			enemy->GetTransform()->SetRotation( rot );
			enemy->GetComponent<hg::BehaviorTree>()->SetEnabled( true );

			hg::Motor* motor = enemy->GetComponent<hg::Motor>();
			motor->SetDesiredForward( -enemy->GetTransform()->Forward() );
			//motor->SetDesiredMove( -enemy->GetTransform()->Forward() );
		}

		++m_CurrRow;

		m_Timer = 0.0f;		
	}

	float halfway = m_Duration * 0.5f;
	if (m_Timer >= halfway)
	{
		Vector3 carrierPos = m_Carrier->GetWorldPosition();
		for (int row = m_CurrRow; row < kRows; ++row)
		{
			for (int col = 0; col < kCols; ++col)
			{
				hg::Entity* enemy = m_Enemies[row * kCols + col];
				
				enemy->GetComponent<hg::Animation>()->Play(SID(run), 1.0f);

				Vector3 startPos = carrierPos;
				startPos.x += -3.7f + col * 0.75f;
				startPos.y += -1.1f;
				startPos.z += -0.7f + (row - (m_CurrRow - 1)) * 0.7f;

				Vector3 endPos = startPos;
				endPos.z = carrierPos.z - 0.7f + (row - m_CurrRow) * 0.7f;

				float r = (m_Timer - halfway) / halfway;
				Vector3 currPos = Vector3::Lerp( startPos, endPos, r );

				enemy->GetTransform()->SetPosition(currPos);
			}
		}		
	}

	return IBehavior::kRUNNING;
}

Hourglass::IBehavior* AI_CarrierReleaseEnemies::MakeCopy() const
{
	AI_CarrierReleaseEnemies* copy = (AI_CarrierReleaseEnemies*)IBehavior::Create( SID(AI_CarrierReleaseEnemies) );
	copy->m_Duration = m_Duration;
	return copy;
}
