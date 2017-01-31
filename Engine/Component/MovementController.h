#pragma once
#include "../Engine/Component/IComponent.h"
namespace Hourglass
{ 
	class MovementController : public IComponent
	{
	public:
		void Init();
		virtual void Update() override;

		void Rotate(float pitch, float yaw, float roll);

		/**
		* Make a copy of this component
		*/
		virtual IComponent* MakeCopyDerived() const;
		virtual int GetTypeID() const  { return s_TypeID; }

	public:
		static uint32_t s_TypeID;
	private:
		void Forward(float speed);
		void Strafe(float speed);
		void Fly(float speed);
		void Yaw(float speed);
		void Pitch(float speed);
	private:
		bool m_moveable;
	};
}