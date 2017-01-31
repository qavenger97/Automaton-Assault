#include "pch.h"
#include "Camera.h"
#include "Core/ComponentFactory.h"
#include "System/CameraSystem.h"
#include "System\PhysicsSystem.h"
#include "Core/TimeManager.h"

uint32_t Hourglass::Camera::s_TypeID = ComponentFactory::GetSystemComponentID();

void Hourglass::Camera::LoadFromXML(tinyxml2::XMLElement* data)
{
	float fov;
	data->QueryFloatAttribute("fov", &fov);
	m_fieldOfView = XMConvertToRadians(fov);
	m_width = (float)hg::CameraSystem::GetScreenWidth();
	m_height = (float)hg::CameraSystem::GetScreenHeight();
	data->QueryFloatAttribute("nearZ", &m_nearZ);
	data->QueryFloatAttribute("farZ", &m_farZ);
	m_aspectRatio = m_width / m_height;
	bool isMain = false;
	data->QueryBoolAttribute("main", &isMain);
	if (isMain)
	{
		hg::CameraSystem::SetMainCamera(this);
	}
	UpdateProjectionMatrix();
	UpdateFrustum();
}

void Hourglass::Camera::Init(float FOV, float Width, float Height, float nearZ, float farZ)
{
	m_fieldOfView = FOV;
	m_width = Width;
	m_height = Height;
	m_nearZ = nearZ;
	m_farZ = farZ;
	m_aspectRatio = m_width / m_height;

	UpdateProjectionMatrix();
	UpdateFrustum();
}

void Hourglass::Camera::SetFieldOfView(const float FOV)
{
	m_fieldOfView = FOV;
	XMMATRIX proj = XMMatrixPerspectiveFovLH(m_fieldOfView, m_height / m_width, m_nearZ, m_farZ);
	XMStoreFloat4x4(&m_projection, proj);
	XMStoreFloat4x4(&m_projection_inverse, XMMatrixInverse(nullptr, proj));
	m_frustum.CreateFromMatrix(m_frustum, proj);
}

XMMATRIX Hourglass::Camera::GetViewMatrix()const
{
	return GetEntity()->GetTransform()->GetInverseMatrix();
}

XMMATRIX Hourglass::Camera::GetViewMatrixInverse()const
{
	return GetEntity()->GetTransform()->GetMatrix();
}

XMMATRIX Hourglass::Camera::GetViewProjectionMatrix()const
{
	XMMATRIX iv = GetEntity()->GetTransform()->GetInverseMatrix();
	XMMATRIX p = XMLoadFloat4x4(&m_projection);
	return iv*p;
}

XMMATRIX Hourglass::Camera::GetViewProjectionInverseMatrix() const
{
	return XMMatrixInverse(nullptr,GetViewProjectionMatrix());
}

XMMATRIX Hourglass::Camera::GetProjectionMatrix()const
{
	return XMLoadFloat4x4(&m_projection);
}

XMMATRIX Hourglass::Camera::GetInverseProjectionMatrix()const
{
	return XMLoadFloat4x4(&m_projection_inverse);
}

XMVECTOR Hourglass::Camera::ScreenToViewport(const FXMVECTOR & position) const
{
	XMVECTOR rs;
	XMVECTOR dimension = XMVectorSet(m_width, 1, m_height, 1);

	rs = (2 * position / dimension) - XMVectorSet(1, 0, 1, 0);
	rs.m128_f32[2] *= -1;
	rs /= XMVectorSet(m_projection(0, 0), 1, m_projection(1, 1), 1);
	return rs;
}

XMVECTOR Hourglass::Camera::ScreenToWorld(const FXMVECTOR & position) const
{
	XMVECTOR rs;
	XMVECTOR dimension = XMVectorSet(m_width*0.5f, m_height*0.5f, 1 , 1);

	XMVECTOR dir = ( position / dimension) - XMVectorSet(1, 1, 0, 0);
	dir.m128_f32[1] *= -1;
	dir.m128_f32[2] = 1;
	//dir /= XMVectorSet(m_projection(0, 0), m_projection(1, 1), 1, 1);
	dir = XMVector4Transform(dir, XMLoadFloat4x4(&m_projection_inverse));
	dir /= XMVectorGetW(dir);
	Camera* camera = CameraSystem::GetMainCamera();
	dir = XMVector4Transform(dir, camera->GetViewMatrixInverse());
	XMVECTOR origin = camera->GetEntity()->GetTransform()->GetWorldPosition();
	
	dir = XMVector3Normalize(dir - origin);
	//rs = origin + dir * position.m128_f32[2];

	//Vector3 vDir;
	//XMStoreFloat3(&vDir, dir);

	//Vector3 ori;
	//XMStoreFloat3(&ori, origin);
	//Ray ray(ori, vDir, 1000);
	//if (g_Physics.RayCast(ray, COLLISION_GROUP_ALL, nullptr, &vDir))
	//	rs = XMLoadFloat3(&vDir);
	//else
	rs = origin + dir;
	return rs;
}

hg::Ray Hourglass::Camera::ScreenToWorldRay(const FXMVECTOR & position) const
{
	XMVECTOR dimension = XMVectorSet(m_width, m_height, 2, 2)*0.5f;

	XMVECTOR dir = (position / dimension) - XMVectorSet(1, 1, 0, 0);
	dir.m128_f32[1] *= -1;
	dir.m128_f32[2] = 1;
	//dir /= XMVectorSet(m_projection(0, 0), m_projection(1, 1), 1, 1);
	dir = XMVector4Transform(dir, XMLoadFloat4x4(&m_projection_inverse));
	dir /= XMVectorGetW(dir);
	Camera* camera = CameraSystem::GetMainCamera();
	dir = XMVector4Transform(dir, camera->GetViewMatrixInverse());
	XMVECTOR origin = camera->GetEntity()->GetTransform()->GetWorldPosition();

	dir = XMVector3Normalize(dir - origin);

	Ray ray(origin, dir, m_farZ);

	return ray;
}

DirectX::XMVECTOR Hourglass::Camera::WorldToScreen(FXMVECTOR position) const
{
	XMVECTOR v = XMVector4Transform(position, GetViewProjectionMatrix());
	v /= XMVectorGetW(v);
	v = XMVectorSetX(v, (XMVectorGetX(v) * 0.5f + 0.5f) * CameraSystem::GetScreenWidth());
	v = XMVectorSetY(v, (1 - (XMVectorGetY(v) * 0.5f + 0.5f)) * CameraSystem::GetScreenHeight());

	return v;
}

void Hourglass::Camera::SetToMainCamera()
{
	Camera* main = CameraSystem::GetMainCamera();
	if(main)
		SetMainCamera(false);
	m_bMainCamera = true;
	CameraSystem::SetMainCamera(this);
}

bool Hourglass::Camera::IsMainCamera() const
{
	return m_bMainCamera;
}

void Hourglass::Camera::Update()
{
	XMStoreFloat3(&m_frustum.Origin, GetEntity()->GetTransform()->GetWorldPosition());
	XMStoreFloat4(&m_frustum.Orientation, GetEntity()->GetTransform()->GetWorldRotation());

	if (m_CameraShakeTimer > 0)
	{
		m_CameraShakeTimer -= hg::g_Time.Delta();

		float t = min(1.0f, m_CameraShakeTimer / m_CameraShakeFadeTime);
		float tx = hg::g_Time.Elapsed() * 17.0f;
		float ty = hg::g_Time.Elapsed() * 23.0f;
		float tr = hg::g_Time.Elapsed() * 29.0f;
		XMMATRIX shake_t = XMMatrixTranslation(m_CameraShakeAmplitude * sinf(tx) * 0.02f * t, m_CameraShakeAmplitude * sinf(ty) * 0.02f * t, 0.0f);
		XMMATRIX shake_r = XMMatrixRotationRollPitchYaw(0, 0, m_CameraShakeRoll * sinf(tr) * 0.02f * t);
		m_CameraShakeMatrix = XMMatrixMultiply(shake_t, shake_r);
	}
	else
	{
		m_CameraShakeMatrix = XMMatrixIdentity();
	}
}

void Hourglass::Camera::OnMainWindowResize(int width, int height)
{
	m_width = (float)width;
	m_height = (float)height;
	m_aspectRatio = m_width / m_height;

	UpdateProjectionMatrix();
	UpdateFrustum();
}

void Hourglass::Camera::ApplyCameraShake(float duration, float fade_time, float amplitude /*= 1.0f*/, float roll /*= 1.0f*/)
{
	m_CameraShakeTimer = duration;
	m_CameraShakeFadeTime = fade_time;
	m_CameraShakeAmplitude = amplitude;
	m_CameraShakeRoll = roll;
}

void Hourglass::Camera::UpdateProjectionMatrix()
{
	XMMATRIX proj = XMMatrixPerspectiveFovLH(m_fieldOfView, m_width / m_height, m_nearZ, m_farZ);
	XMStoreFloat4x4(&m_projection, proj);
	XMStoreFloat4x4(&m_projection_inverse, XMMatrixInverse(nullptr, proj));
}

void Hourglass::Camera::UpdateFrustum()
{
	XMMATRIX proj = XMLoadFloat4x4(&m_projection);
	m_frustum.CreateFromMatrix(m_frustum, proj);
}

Hourglass::IComponent* Hourglass::Camera::MakeCopyDerived() const
{
	// TODO: Make a copy of this component
	// Do not bother initializing any values that the Init function will modify,
	// as Init will probably need to be called after multiple components are copied
	Camera* cpy = (Camera*)IComponent::Create( SID(Camera) );
	cpy->Init(m_fieldOfView, m_width, m_height, m_nearZ, m_farZ);
	cpy->SetToMainCamera();
	return cpy;
	
	//return IComponent::Create("Camera");
}
