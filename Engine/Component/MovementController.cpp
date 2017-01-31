#include "pch.h"
#include "MovementController.h"
#include "Core/Entity.h"
#include "Core/InputManager.h"
#include "Core/ComponentFactory.h"
#include "Core/TimeManager.h"
uint32_t Hourglass::MovementController::s_TypeID = ComponentFactory::GetSystemComponentID();
void Hourglass::MovementController::Init()
{
	m_moveable = true;
}
void Hourglass::MovementController::Update()
{
	float speed = m_moveable? 3.0f * g_Time.UnscaledDelta() : 0;
	if (g_Input.IsKeyDown(VK_RBUTTON))
	{
		int rx, ry;
		g_Input.GetCursorRelPos(rx, ry);
	
		Pitch(XMConvertToRadians((float)ry) );
		Yaw(XMConvertToRadians((float)rx) );
	}

	if (g_Input.GetBufferedKeyState(VK_F4) == hg::BufferedKeyState::kKeyStateReleased)
		m_moveable = !m_moveable;

	if (g_Input.IsKeyDown(VK_SHIFT)) speed *= 5.0f;
	if (g_Input.IsKeyDown('W')) Forward(speed);
	if (g_Input.IsKeyDown('S')) Forward(-speed);
	if (g_Input.IsKeyDown('A')) Strafe(-speed);
	if (g_Input.IsKeyDown('D')) Strafe(speed);
	if (g_Input.IsKeyDown(VK_SPACE)) Fly(speed);
	if (g_Input.IsKeyDown(VK_CONTROL)) Fly(-speed);
}

void Hourglass::MovementController::Rotate(float pitch, float yaw, float roll)
{
	GetEntity()->GetTransform()->LocalRotation(pitch, yaw, roll);
}
void Hourglass::MovementController::Forward(float speed)
{
	XMVECTOR velocity = XMVectorSet(0, 0, speed, 1);
	XMVECTOR dir = GetEntity()->GetTransform()->GetWorldRotation();
	velocity = XMVector3Rotate(velocity, dir);
	GetEntity()->GetTransform()->Translate(velocity);
}

void Hourglass::MovementController::Strafe(float speed)
{
	XMVECTOR velocity = XMVectorSet(speed, 0, 0 ,1);
	XMVECTOR dir =GetEntity()->GetTransform()->GetWorldRotation();
	velocity = XMVector3Rotate(velocity, dir);
	GetEntity()->GetTransform()->Translate(velocity);
}

void Hourglass::MovementController::Fly(float speed)
{
	XMVECTOR velocity = XMVectorSet(0, speed, 0, 1);
	GetEntity()->GetTransform()->Translate(velocity);
}

void Hourglass::MovementController::Yaw(float speed)
{	
	
	GetEntity()->GetTransform()->LocalRotation(0, speed, 0);
}

void Hourglass::MovementController::Pitch(float speed)
{
	GetEntity()->GetTransform()->Rotate(speed, 0, 0);
}

Hourglass::IComponent* Hourglass::MovementController::MakeCopyDerived() const
{
	// TODO: Make a copy of this component
	// Do not bother initializing any values that the Init function will modify,
	// as Init will probably need to be called after multiple components are copied
	return IComponent::Create( SID(MovementController) );
}

