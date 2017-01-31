#pragma once
#include "../Engine/Component/IComponent.h"
#include "../Core/Math.h"
#include "../Core/Entity.h"
#include "../Core/InputManager.h"
namespace Hourglass
{
	class Ray;
	class Camera : public IComponent
	{
	public:
		void Init() {}
		void LoadFromXML(tinyxml2::XMLElement * data);
		void Init(const float FOV, const float Width, const float Height, const float nearZ, const float farZ);
		void SetFieldOfView(const float FOV);
		XMMATRIX GetViewMatrix()const;
		XMMATRIX GetViewMatrixInverse()const;
		XMMATRIX GetViewProjectionMatrix()const;
		XMMATRIX GetViewProjectionInverseMatrix()const;
		XMMATRIX GetProjectionMatrix()const;
		XMMATRIX GetInverseProjectionMatrix()const;
		XMMATRIX GetCameraShakeMatrix() const { return m_CameraShakeMatrix; }
		BoundingFrustum const& GetViewFrustum() { return m_frustum; }
		/**Use x and z for 
		*
		*/
		XMVECTOR ScreenToViewport(const FXMVECTOR& position)const;
		XMVECTOR ScreenToWorld(const FXMVECTOR& position)const;
		Ray ScreenToWorldRay(const FXMVECTOR& position)const;
		XMVECTOR WorldToScreen(FXMVECTOR position) const;

		void SetToMainCamera();
		void SetMainCamera(bool main) { m_bMainCamera = main; }
		inline float AspectRatio()const { return m_aspectRatio; }
		bool IsMainCamera()const;
		virtual void Update() override;
		void OnMainWindowResize(int width, int height);

		void ApplyCameraShake(float duration, float fade_time, float amplitude = 1.0f, float roll = 1.0f);

		virtual int GetTypeID() const { return s_TypeID; }
		/**
		* Make a copy of this component
		*/
		virtual IComponent* MakeCopyDerived() const override;

	private:
		void UpdateProjectionMatrix();
		void UpdateFrustum();

	public:
		static uint32_t s_TypeID;
	private:
		BoundingFrustum m_frustum;
		XMFLOAT4X4 m_projection;
		XMFLOAT4X4 m_projection_inverse;
		float m_fieldOfView;
		float m_width;
		float m_height;
		float m_nearZ;
		float m_farZ;
		float m_aspectRatio;
		bool m_bMainCamera;

		XMMATRIX m_CameraShakeMatrix;
		float m_CameraShakeTimer;
		float m_CameraShakeFadeTime;
		float m_CameraShakeAmplitude;
		float m_CameraShakeRoll;
	};
}
