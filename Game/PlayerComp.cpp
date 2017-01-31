#include "GamePch.h"

#include "PlayerComp.h"
#include "ThirdPersonCameraComponent.h"
#include "ComponentSet.h"
#include "Common.h"
#include "HealthModule.h"
#include "Renderer\DevTextRenderer.h"
#include "../Engine/System/EntityManager.h"
#include "Renderer\Texture.h"

uint32_t PlayerComp::s_TypeID = hg::ComponentFactory::GetGameComponentID();


extern hg::Texture* g_ScreenOverlay;
extern Color g_ScreenOverlayColor;


void PlayerComp::LoadFromXML( tinyxml2::XMLElement* data )
{
	data->QueryFloatAttribute( "speed", &m_Speed );	
	m_RunSpeed = m_Speed;
	data->QueryFloatAttribute( "dodge_speed", &m_DodgeSpeed );
	data->QueryFloatAttribute( "dodge_duration", &m_DodgeDuration ); 
	m_DodgeTime = 0;
}

void PlayerComp::Init()
{
	m_Anims.Idle	 = SID(idle);
	m_Anims.Fire	 = SID(fire);
	m_Anims.FwdLeft	 = SID(fwd_left);
	m_Anims.FwdRight = SID(fwd_right);
	m_Anims.Bwd		 = SID(bwd);
	m_Anims.BwdLeft	 = SID(bwd_left);
	m_Anims.BwdRight = SID(bwd_right);
	m_Anims.Right	 = SID(right);
	m_Anims.Left	 = SID(left);
	m_Anims.Fwd		 = SID(fwd);
	m_Anims.Dodge    = SID(dodge);

	m_MovementLock = false;

	m_curGun = 0;
	upperRotOffset = XMQuaternionRotationRollPitchYaw(0.0, 0.8f, 0.0f);
	m_debug = false;
	m_freeAim = false;
	m_needRotate = false;
	Integrate = Math::NoGravity;
	m_powerUp = PowerUpType::kNoPowerUp;
}

void PlayerComp::Start()
{
	m_rollTimer = 0;
	m_DamageOverlay = 0.0f;
	m_powerUp = PowerUpType::kNoPowerUp;
	m_powerUpAmount = 0;
	g_ScreenOverlay = hg::g_TextureManager.GetTexture("Assets/Texture/BloodOverlay.dds");
	g_ScreenOverlayColor = Color(0, 0, 0, 0);
}

void PlayerComp::Update()
{
	if (m_DamageDecayTimer > 0.0f)
	{
		m_DamageDecayTimer -= hg::g_Time.Delta();
	}
	else if (m_DamageOverlay > 0.0f)
	{
		m_DamageOverlay -= hg::g_Time.Delta();
		m_DamageOverlay = max(0.0f, m_DamageOverlay);
		g_ScreenOverlayColor = Color(1, 1, 1, m_DamageOverlay);
	}

	hg::Entity& entity = *GetEntity();
	hg::DynamicCollider& dc = *entity.GetComponent<hg::DynamicCollider>();
	float dt = hg::g_Time.Delta();

	if (m_DodgeTime && hg::g_Time.Elapsed() >= m_DodgeTime)
	{
		EndDodge();
	}

	if (hg::g_Time.GetTimeScale() == 0)
	{
		m_gun[m_curGun]->GetComponent<GunLogic>()->Cease();
		return;
	}

	

	XMFLOAT3 moveDir = { 0.0f, 0.0f, 0.0f };

	bool isMoving = false;

	if (m_DodgeTime == 0.0)
	{

#ifdef FINAL
		if (hg::g_Input.GetBufferedKeyState( VK_F3 ) == hg::BufferedKeyState::kKeyStateReleased)
		{
			m_debug = !m_debug;
		}

		if (hg::g_Input.GetBufferedKeyState( VK_F6 ) == hg::BufferedKeyState::kKeyStateReleased)
			Integrate = Math::Gravity;
		if (hg::g_Input.GetBufferedKeyState( VK_F7 ) == hg::BufferedKeyState::kKeyStateReleased)
			Integrate = Math::NoGravity;
#endif
		
		if (hg::g_Input.IsKeyDown( 'W' ))
		{
			moveDir.z += 1;
		}
		if (hg::g_Input.IsKeyDown( 'A' ))
		{
			moveDir.x -= 1;
		}
		if (hg::g_Input.IsKeyDown( 'S' ))
		{
			moveDir.z -= 1;
		}
		if (hg::g_Input.IsKeyDown( 'D' ))
		{
			moveDir.x += 1;
		}
	}
	

	Vector3 moveDirection = XMVector3Normalize( XMLoadFloat3( &moveDir ) );

	Vector3 trueMoveDirection = moveDirection;

	if (m_DodgeTime == 0)
	{
		m_MoveDirection = moveDirection;
	}
	else
	{
		moveDirection = m_MoveDirection;
	}

	float speed = dt * m_Speed;

	XMStoreFloat3(&moveDir, moveDirection);
	XMVECTOR move = moveDirection* speed;

#ifdef FINAL
	if (hg::g_Input.IsKeyDown(VK_BACK))
	{
		GetEntity()->GetTransform()->SetPosition(0, 0, 0);
	}
#endif

	if (hg::g_Input.IsKeyDown( VK_SPACE ))
	{
		Dodge(moveDirection);
	}

	if (Integrate)
	{
		Integrate(XMVectorZero(), move, dt, G_GRAVITY);
		XMStoreFloat3( &moveDir, move );

		if (moveDir.x != 0.0f || moveDir.y != 0.0f || moveDir.z != 0.0f)
		{
			isMoving = true;
			m_needRotate = true;
		}

		if (!m_MovementLock)
			dc.Move(move);
	}

	int mouseX, mouseY;
	hg::g_Input.GetCursorWindowPos( mouseX, mouseY );
	XMVECTOR mouseOffset = XMVectorSet( (float)mouseX, 0, (float)mouseY, 0 );
	hg::Camera* main = hg::CameraSystem::GetMainCamera();;
	XMVECTOR dir = main->ScreenToViewport( mouseOffset );
	dir = XMVector3ClampLength( dir, 0, 0.5f ) * 15;
	ThirdPersonCameraComponent* tcc = hg::g_cameraSystem.GetCamera(SID(Game Camera))->GetEntity()->GetComponent<ThirdPersonCameraComponent>();
	tcc->SetDesireOffsetDistance(dir);
	
	if(tcc)
		tcc->SetDesireOffsetDistance(dir);
	
	tcc->SetDesireOffsetDistance( dir );

	int x, y;
	hg::g_Input.GetCursorWindowPos( x, y );
	XMVECTOR pos_mouse_world = XMVectorSet( (float)x, (float)y, 1, 1 );

	hg::Ray ray = hg::CameraSystem::GetMainCamera()->ScreenToWorldRay( pos_mouse_world );
#if _DEBUG
	hg::DebugRenderer::DrawAabb(hg::Aabb(m_gun[m_curGun]->GetTransform()->GetWorldPosition()+XMVectorSet(-.5f, -.5f, -.5f,0), m_gun[m_curGun]->GetTransform()->GetWorldPosition()+XMVectorSet(.5f, .5f, .5f,0)));
#endif
	hg::Transform* trans = GetEntity()->GetTransform();

	// Aim at a target or a direction
	Vector3 hit;
	hg::Entity* target;
	bool noEnemyHit = true;
	bool noHit = true;
	if (hg::g_Physics.RayCast(ray, &target, &hit, nullptr, ~(COLLISION_UNWALKABLE_MASK | COLLISION_ENEMY_ONLY_OBSTACLE_MASK)))
	{
		
		pos_mouse_world = XMLoadFloat3(&hit);
		noHit = false;
		float newDt = dt * 5;
		
		if (target->GetTag() == SID(Enemy) || target->GetTag() == SID(Breakable))
		{
			upper->TurnTo(target->GetTransform()->GetWorldPosition(), newDt);

			noEnemyHit = false;
		}
		else if(m_freeAim)
		{
			upper->TurnTo( pos_mouse_world, newDt );

			noEnemyHit = false;
		}
	}

	m_PosMouseWorld = pos_mouse_world;
	m_PosMouseWorld.y = 0.0f;

	float hitTime;
	XMVECTOR upperPos = upper->GetWorldPosition();
	hg::g_Physics.LinePlaneTest(ray.Origin, ray.Direction, G_UP, XMVectorGetY(upperPos), &hitTime);
	XMVECTOR lookPoint = ray.Origin + ray.Direction * -hitTime;
	XMVECTOR rotation = hg::Transform::BuildLookAtRotation(upperPos, lookPoint);
	

	if (noEnemyHit)
	{
		upper->RotateTo(rotation, dt * 8);
	}

	XMVECTOR upperForward = upper->Forward();
	upperForward = XMVectorSetY(upperForward, 0);
	XMVECTOR bodyforward = trans->Forward();

	float angleBetweenBody = XMVectorGetX(XMVector3Dot(upperForward, bodyforward));
	float angleBetweenMoveForward = XMVectorGetX(XMVector3Dot(upperForward, moveDirection));
	//float angleBetweenMoveRight = XMVectorGetX(XMVector3Dot(moveDirection, upper->Right()));
	

	if(angleBetweenMoveForward < 0)
	{
		moveDirection *= -1;
	}

	if(angleBetweenBody <= 0.3f)
	{
		m_needRotate = true;
	}
	if ((1 - angleBetweenBody) < 0.001f && !isMoving)
	{
		m_needRotate = false;
	}
	if (m_DodgeTime)
	{
		m_needRotate = false;
	}
#if _DEBUG
	hg::DebugRenderer::DrawLine(upper->GetWorldPosition(), upper->GetWorldPosition() + upper->Forward() * 2, Color(0, 0, 1), Color(0, 0, 1));
	hg::DebugRenderer::DrawLine(upper->GetWorldPosition(), upper->GetWorldPosition() + upper->Right() * 2, Color(1, 0, 0), Color(1, 0, 0));
	hg::DebugRenderer::DrawLine(upper->GetWorldPosition(), upper->GetWorldPosition() + upper->Up() * 2, Color(0, 1, 0), Color(0, 1, 0));
	hg::DebugRenderer::DrawLine(trans->GetWorldPosition(), trans->GetWorldPosition() + bodyforward * 2, Color(0, 1, 0), Color(0, 1, 0));
#endif
	if (m_needRotate)
	{
		XMVECTOR rot = trans->GetWorldRotation();;
		XMVECTOR newrot;

		if (isMoving)
		{
			if (angleBetweenMoveForward < 0.25f && angleBetweenMoveForward > 0)
			{
				newrot = hg::Transform::BuildLookAtRotationFromDirectionVector(upperForward);

				
				newrot = XMQuaternionSlerp(rot, newrot, dt*6);

				trans->SetRotation(newrot);
				rot = XMQuaternionMultiply(XMQuaternionConjugate(rot), newrot);
				upper->Rotate(XMQuaternionConjugate(rot));
			}

			


			XMVECTOR moveRot = hg::Transform::BuildLookAtRotationFromDirectionVector(moveDirection);

			rot = trans->GetWorldRotation();

			/*float bodyDir = XMVectorGetX(XMVector4Dot(rot, moveRot));
			float lookDir = XMVectorGetX(XMVector4Dot(rotation, moveRot));
			if (bodyDir < 0 && lookDir < 0)
			{
				moveRot *= -1;
			}*/

			//XMVECTOR uppperRot = upper->GetWorldRotation();
			newrot = XMQuaternionSlerp(rot, moveRot, dt * 6);

#if _DEBUG
			hg::DebugRenderer::DrawLine(upper->GetWorldPosition(), upper->GetWorldPosition() + XMVector3Rotate(G_FORWARD, newrot) * 5, Color(1, 0, 1), Color(1, 0, 1));
#endif
			float comp = XMVectorGetX(XMVector3Dot(upperForward, XMVector3Rotate(G_FORWARD, newrot)));

			if (m_DodgeTime == 0.0f)
			{
				trans->SetRotation( newrot );
			}
			else
			{
				trans->SetRotation( moveRot );
			}
			rot = XMQuaternionMultiply(XMQuaternionConjugate(newrot), rot);
			upper->Rotate(rot);
		}
		else
		{
			newrot = hg::Transform::BuildLookAtRotationFromDirectionVector(upperForward);
			
			newrot = XMQuaternionSlerp(rot, newrot, dt * 6);

			if (m_DodgeTime == 0.0f)
			{
				trans->SetRotation( newrot );
			}
			rot = XMQuaternionMultiply(XMQuaternionConjugate(newrot), rot);
			upper->Rotate(rot);
		}
		
	}

#ifdef FINAL
	if (hg::g_Input.GetBufferedKeyState(VK_RBUTTON) == hg::BufferedKeyState::kKeyStateReleased)
	{
		if(hg::g_Input.IsKeyDown(VK_LCONTROL))
			if (noHit)
				GetEntity()->GetTransform()->SetPosition(lookPoint);
			else
			{
				GetEntity()->GetTransform()->SetPosition(pos_mouse_world);
				Vector3 pos = GetEntity()->GetComponent<hg::DynamicCollider>()->GetGroundedPosition();
				GetEntity()->GetTransform()->SetPosition(pos);
			}
	}
#endif

	HandleControl();
	// Calculate Legs Animation

	//upperBone->SetRotation(XMQuaternionIdentity());
	m_rollTimer = max(m_rollTimer - dt, 0);
	if (m_DodgeTime == 0.0f)
	{
		if (isMoving)
		{
			m_needRotate = true;
			if (angleBetweenMoveForward > 0)
			{
				if (!animation->IsPlaying( m_Anims.Fwd ))
				{
					animation->Play( m_Anims.Fwd, 1 );
				}
			}
			else if (angleBetweenMoveForward <= 0)
			{
				if (!animation->IsPlaying( m_Anims.Bwd ))
				{
					animation->Play( m_Anims.Bwd, 0.6f );
				}
			}

			if (!animation->IsPlaying( m_Anims.Idle ))
				animation->Play( m_Anims.Idle, 1, 1 );
		}
		else
		{
			animation->Stop();
			if (!animation->IsPlaying())
				animation->Play( m_Anims.Idle, 1 );
		}

		XMVECTOR ori = XMQuaternionMultiply( upperRotOffset, upper->GetLocalRotation() );
		animation->GetBoneTransform( 0 ).SetRotation( 0, 0, 0 );
		upperBone->Rotate( ori );

		
		if (!(Vector3::Forward.Dot( upper->Forward() ) > 0 && (trueMoveDirection.z == 1.0 || trueMoveDirection.z == -1.0)))
		{
			animation->GetBoneTransform( 0 ).Rotate( 0, XM_PI, 0 );
			upperBone->LocalRotation( 0, -XM_PI, 0 );			
		}

		animation->GetBoneTransform( 1 ).Rotate( -0.35f, 0, 0 );
		
		XMFLOAT3 hipWPos;
		XMStoreFloat3(&hipWPos, animation->GetBoneTransform( 0 ).GetWorldPosition());
		animation->GetBoneTransform( 0 ).SetPosition( 0, hipWPos.y - 100.0f, 0 );
		upperBone->Rotate( 0, XM_PI, 0 );

		GunLogic* currGun = m_gun[m_curGun]->GetComponent<GunLogic>();
		unsigned int gunState = currGun->GetGunState();
		//if (gunState == GunLogic::State::FIREING)
		//{
		//	if (!animation->IsPlaying( m_Anims.Fire ))
		//	{
		//		animation->Play( m_Anims.Fire, 1.0f, 1 );
		//	}
		//}
	}
	else
	{
		if (!animation->IsPlaying())
			animation->Play( m_Anims.Idle, 1 );
		animation->GetBoneTransform( 0 ).SetPosition( 0, animation->GetBoneTransform( 0 ).GetWorldPosition().m128_f32[1], 0 );
	}

	// Make sure gun is attached to character's hand
	UpdateGunTransform();
}

void PlayerComp::OnMessage(hg::Message* msg)
{
	if (hg::g_Time.GetTimeScale() == 0)return;
	switch (msg->GetType())
	{
	case GameMessageType::kDamage:
		{
			if (IsPlayerAlive())
			{
				if (m_powerUp & PowerUpType::kPowerUp_Shield)
				{
					break;
				}
				DamageMessage* dmgMsg = (DamageMessage*)msg;
				if (dmgMsg->GetDamageType() == kDmgType_Bullet)
					hg::g_AudioSystem.PostEvent(AK::EVENTS::PLAY_PLAYER_BULLETHIT, GetEntity());

				hg::g_AudioSystem.PostEvent(AK::EVENTS::PLAY_PLAYER_PAIN, GetEntity());

				// Apply camera shake
				hg::g_cameraSystem.GetMainCamera()->ApplyCameraShake(0.5f, 0.2f, 2.0f, 0.0f);

				m_DamageOverlay += 0.1f;
				m_DamageOverlay = min(1.0f, m_DamageOverlay);
				g_ScreenOverlayColor = Color(1, 1, 1, m_DamageOverlay);
				m_DamageDecayTimer = 1.0f;
			}

			break;
		}
	case GameMessageType::kDeath:
		{
			// Stop camera shake
			hg::g_cameraSystem.GetMainCamera()->ApplyCameraShake(0, 0);
			g_ScreenOverlayColor = Color(0, 0, 0, 0);

			PlayerDeathMessage newMsg;
			if(!m_debug)
				hg::g_EntityManager.BroadcastMessage(&newMsg);

			break;
		}
	case GameMessageType::kPowerUp:
		{
		PowerUpMessage* inMsg = (PowerUpMessage*)msg;
			UINT _newType = inMsg->GetPowerUpType();

			if (_newType & PowerUpType::kPowerUp_End)
			{
				if (_newType & PowerUpType::kPowerUp_Shield)
				{
					m_powerUp &= ~PowerUpType::kPowerUp_Shield;
					m_health->SetInvulnerable(false);
				}
			}
			else if (_newType & PowerUpType::kPowerUp_Ricochet)
			{
				m_powerUpAmount = (int)inMsg->GetAmount();
				m_powerUp |= PowerUpType::kPowerUp_Ricochet;
				for (int i = 0; i < 2; i++)
				{
					m_gun[i]->GetComponent<GunLogic>()->SetBounce(true);
				}
			}
			else if (_newType & PowerUpType::kPowerUp_Shield)
			{
				hg::Transform* trans = GetEntity()->GetTransform()->FindChild(SID(FX_Shield));
				hg::Entity* shield = nullptr;
				if (trans)
				{
					shield = trans->GetEntity();
					shield->Destroy();
				}

				m_powerUp |= PowerUpType::kPowerUp_Shield;
				shield = hg::Entity::Assemble(SID(FX_Shield), GetEntity()->GetTransform()->GetWorldPosition(), Quaternion::Identity);
				shield->GetTransform()->LinkTo(GetEntity()->GetTransform());
				m_health->SetInvulnerable(true);

			}
			hg::Entity::FindByName(SID(MainMenu))->SendMsg(msg);
		}
		break;
	case GameMessageType::kFire:
		m_powerUpAmount--;
		if (m_powerUpAmount <= 0)
		{
			m_powerUp &= ~PowerUpType::kPowerUp_Ricochet;
			PowerUpMessage newMsg(PowerUpType::kPowerUp_End | PowerUpType::kPowerUp_Ricochet, 0);
			hg::Entity::FindByName(SID(MainMenu))->SendMsg(&newMsg);
			for (int i = 0; i < 2; i++)
			{
				m_gun[i]->GetComponent<GunLogic>()->SetBounce(false);
			}
		}
		break;
	}
}

void PlayerComp::SetGun(hg::Entity * gun, int index)
{
	if (index < MAX_NUM_GUN && index >= 0)
	{
		m_gun[index] = gun;

		// Detach gun from player and move to world entity
		gun->GetTransform()->Reset();

		if (m_curGun != index)
			m_gun[index]->GetComponent<hg::MeshRenderer>()->SetEnabled(false);
	}
}

void PlayerComp::PostInit()
{
	animation = GetEntity()->GetComponent<hg::Animation>();
	upper = GetEntity()->GetTransform()->FindChild(SID(Upper));
	dodgeBody = GetEntity()->GetTransform()->FindChild( SID(DodgeBody) );
	upperBone = &animation->GetBoneTransform(2);
	m_health = GetEntity()->GetComponent<Health>();
	gunHeight = XMVectorGetY(m_gun[m_curGun]->GetTransform()->GetWorldPosition());
}

float PlayerComp::GetCurrentAmmoRatio() const
{
	return m_gun[m_curGun]->GetComponent<GunLogic>()->GetAmmoRatio();
}

float PlayerComp::GetCurrentHealthRatio() const
{
	return m_health ? m_health->GetHealthRatio() : 1.0f;
}

float PlayerComp::GetCurrentTotalAmmoRatio() const
{
	return m_gun[m_curGun]->GetComponent<GunLogic>()->GetTotalAmmoRatio();
}

void PlayerComp::HandleControl()
{
	if (hg::g_Input.GetBufferedKeyState(VK_LBUTTON) == hg::BufferedKeyState::kKeyStatePressed)
	{
		if (!IsDodging())
		{
			m_gun[m_curGun]->GetComponent<GunLogic>()->Fire();
		}
	}
	if (hg::g_Input.GetBufferedKeyState(VK_LBUTTON) == hg::BufferedKeyState::kKeyStateReleased)
	{
		m_gun[m_curGun]->GetComponent<GunLogic>()->Cease();
	}
	if (hg::g_Input.GetBufferedKeyState('R') == hg::BufferedKeyState::kKeyStateReleased)
	{
		m_gun[m_curGun]->GetComponent<GunLogic>()->Reload();
	}
	if (hg::g_Input.GetBufferedKeyState(VK_SHIFT) == hg::BufferedKeyState::kKeyStateDown)
	{
		m_freeAim = true;
	}
	if (hg::g_Input.GetBufferedKeyState(VK_SHIFT) == hg::BufferedKeyState::kKeyStateReleased)
	{
		m_freeAim = false;
	}
	if (hg::g_Input.GetBufferedKeyState('1') == hg::BufferedKeyState::kKeyStateReleased)
	{
		SwitchToWeapon(0);
	}
	if (hg::g_Input.GetBufferedKeyState('2') == hg::BufferedKeyState::kKeyStateReleased)
	{
		SwitchToWeapon(1);
	}

#ifdef FINAL
	if (hg::g_Input.GetBufferedKeyState(VK_OEM_PLUS) == hg::BufferedKeyState::kKeyStatePressed)
	{
		//Vector3 pos = Math::RNG::RandomVectorRange(Vector3(0, 0, 0), Vector3(6, 6, 6));
		XMVECTOR posOri = GetEntity()->GetTransform()->GetWorldPosition();
		XMVECTOR pos = GetEntity()->GetTransform()->GetWorldPosition() + XMVectorSet(0, 0, 1, 0);
		XMVECTOR pos0 = GetEntity()->GetTransform()->GetWorldPosition() - XMVectorSet(0, 0, 1, 0);
		XMVECTOR pos1 = GetEntity()->GetTransform()->GetWorldPosition() - XMVectorSet(1, 0, 0, 0);
		/*hg::Entity::Assemble(SID(FX_Ground000), pos, Quaternion::Identity);
		hg::Entity::Assemble(SID(FX_Explosion), pos, Quaternion::Identity);
		hg::Entity::Assemble(SID(FX_ExplosionSpark), pos, Quaternion::Identity);
		hg::Entity::Assemble(SID(FX_ExplosionSpark), pos, Quaternion::Identity);*/
		//hg::Entity::Assemble(SID(FX_DirtDig), pos, Quaternion::Identity);
		hg::Entity::Assemble(SID(AmmoPack), pos, Quaternion::Identity);
		hg::Entity::Assemble(SID(MedPack), pos0, Quaternion::Identity);
		hg::Entity::Assemble(SID(Pickup_Ricochet), pos1, Quaternion::Identity);
		//hg::Entity* shield = hg::Entity::Assemble(SID(FX_Shield), posOri, Quaternion::Identity);
		hg::Entity::Assemble(SID(Pickup_Shield), pos1 - XMVectorSet(1,0,0,0), Quaternion::Identity);
		/*PowerUpMessage pmsg(PowerUpType::kPowerUp_Shield, 5);
		GetEntity()->SendMsg(&pmsg);*/
		//shield->GetTransform()->LinkTo(GetEntity()->GetTransform());
		m_health->SetValue(50);
	}
#endif
	//if (hg::g_Input.GetBufferedKeyState('T') == hg::BufferedKeyState::kKeyStatePressed)
	//{
	//	if (m_curGun == 0)
	//	{
	//		GunLogic* gun = m_gun[m_curGun]->GetComponent<GunLogic>();
	//		gun->ToggleBouncingBullet(!gun->IsUsingBouncingBullet());
	//	}
	//}
}

void PlayerComp::SwitchToWeapon(int slot)
{
	m_gun[m_curGun]->GetComponent<GunLogic>()->Cease();
	m_gun[m_curGun]->GetComponent<hg::MeshRenderer>()->SetEnabled(false);
	m_curGun = slot;
	m_gun[m_curGun]->GetComponent<hg::MeshRenderer>()->SetEnabled(true);
}

void PlayerComp::Dodge(const Vector3 moveDir)
{
	if (m_DodgeTime == 0 && m_rollTimer <=0)
	{
		m_Speed = m_DodgeSpeed;
		float elapsed = hg::g_Time.Elapsed();
		m_DodgeTime = elapsed + m_DodgeDuration;
		GetEntity()->RemoveComponent( animation );
		hg::SkinnedMeshRenderer* mr = GetEntity()->GetComponent<hg::SkinnedMeshRenderer>();
		GetEntity()->RemoveComponent( mr );
		mr->SetAlive( true );
		mr->SetEnabled( true );
		animation->SetAlive( true );
		animation->SetEnabled( true );
		hg::Entity* db = dodgeBody->GetEntity();
		db->AttachComponent( animation );
		db->AttachComponent( mr );

		animation->Play( m_Anims.Dodge, 1 );
		
		if (moveDir.z == 1.0f)
			db->GetTransform()->SetRotation( 0.0f, XM_PI, 0.0f );
		else if (moveDir.z == -1.0f)
			db->GetTransform()->SetRotation( 0.0f, 0.0f, 0.0f );
		else if (moveDir.x == 1.0f)
			db->GetTransform()->SetRotation( 0.0f, XM_PI + XM_PIDIV2, 0.0f );
		else if (moveDir.x == -1.0f)
			db->GetTransform()->SetRotation( 0.0f, XM_PIDIV2, 0.0f );
		else if (moveDir.x > 0)
		{
			if (moveDir.z > 0)
			{
				db->GetTransform()->SetRotation( 0.0f, XM_PI + XM_PIDIV4, 0.0f );
			}
			else
			{
				db->GetTransform()->SetRotation( 0.0f, -XM_PIDIV4, 0.0f );
			}
		}
		else if (moveDir.z > 0)
			db->GetTransform()->SetRotation( 0.0f, XM_PI - XM_PIDIV4, 0.0f );
		else
			db->GetTransform()->SetRotation( 0.0f, XM_PIDIV4, 0.0f );

		//m_MovementLock = true;

		// Cease fire
		m_gun[m_curGun]->GetComponent<GunLogic>()->Cease();

		// Hide gun and laser
		m_gun[m_curGun]->GetComponent<hg::MeshRenderer>()->SetEnabled(false);
		GetGunLaser()->GetComponent<hg::MeshRenderer>()->SetEnabled(false);
	}
}

void PlayerComp::EndDodge()
{
	m_rollTimer = 2;
	m_Speed = m_RunSpeed;
	m_DodgeTime = 0;
	m_MovementLock = false;

	hg::Entity* db = dodgeBody->GetEntity();
	db->RemoveComponent( animation );
	hg::SkinnedMeshRenderer* mr = db->GetComponent<hg::SkinnedMeshRenderer>();
	db->RemoveComponent( mr );
	mr->SetAlive( true );
	mr->SetEnabled( true );
	animation->SetAlive( true );
	animation->SetEnabled( true );
	
	GetEntity()->AttachComponent( animation );
	GetEntity()->AttachComponent( mr );

	// Show gun and laser
	m_gun[m_curGun]->GetComponent<hg::MeshRenderer>()->SetEnabled(true);
	GetGunLaser()->GetComponent<hg::MeshRenderer>()->SetEnabled(true);
}

bool PlayerComp::IsDodging() const
{
	return m_DodgeTime && hg::g_Time.Elapsed() < m_DodgeTime;
}

bool PlayerComp::RestoreHP(float value)
{
	return m_health->RestoreHealth(value);
}

bool PlayerComp::RestoreAmmo(float value)
{
	return m_gun[m_curGun]->GetComponent<GunLogic>()->AddAmmo((int)value);
}

bool PlayerComp::IsPlayerAlive() const
{
	Health* health = GetEntity()->GetComponent<Health>();
	if (!health) return false;

	return health->GetValue() > 0;
}

hg::Entity* PlayerComp::GetGunLaser()
{
	return GetEntity()->GetTransform()->FindChild(SID(Upper))->FindChild(SID(GunPos))->GetEntity();
}

hg::IComponent* PlayerComp::MakeCopyDerived() const
{
	// TODO: Make a copy of this component
	// Do not bother initializing any values that the Init function will modify,
	// as Init will probably need to be called after multiple components are copied
	return hg::IComponent::Create( SID(PlayerComp) );
}

void PlayerComp::UpdateGunTransform()
{
	Matrix t = GetEntity()->GetTransform()->GetMatrix();
	//hg::LoadingSkeleton* skel = animation->GetLoadingSkeleton();
	//for (int i = 0; i < 65; i++)
	//{
	//	int p = skel->m_ParentIndices[i];
	//	if (p < 0)
	//		continue;

	//	Matrix m = animation->GetBoneWorldPose(i) * t;
	//	Matrix mp = animation->GetBoneWorldPose(p) * t;
	//	//hg::DebugRenderer::DrawAxis(m);
	//	hg::DebugRenderer::DrawLine(mp.Translation(), m.Translation());
	//}

	static int kRightHandBoneId = 34;	// Note: change bone index if use different player model

	hg::Transform* gunTrans = m_gun[m_curGun]->GetTransform();
	Vector3 hand_scale, hand_pos;
	Quaternion hand_rot;
	Matrix hand_mat = animation->GetBoneWorldPose(kRightHandBoneId) * t;

	if (hand_mat.Decompose(hand_scale, hand_rot, hand_pos))
	{
		gunTrans->SetScale(hand_scale);
		gunTrans->SetPosition(hand_pos);
		gunTrans->SetRotation(hand_rot);
		gunTrans->CheckAndUpdate();
	}
}
