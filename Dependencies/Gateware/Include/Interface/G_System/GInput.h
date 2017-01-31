#ifndef _GINPUT_H
#define _GINPUT_H

#include "../G_Core/GSingleThreaded.h"
#include "../G_System/GKeyDefines.h"
#include <Windows.h>

namespace GW
{
	namespace CORE
	{

		static const GUUIID GInputUUIID =
		{
			0x4cba9d69, 0x1b32, 0x43da,{ 0xb7, 0xb2, 0xa4, 0x21, 0xc5, 0x78, 0x18, 0xf0 }
		};

		//! A single threaded input library.
		/*!
		* The single thread input library is used for high speed game input.
		* You can use this library to get any mouse or keyboard input.
		*/
		class GInput : public GSingleThreaded
		{

		public:

			//! Get the current state of any key.
			/*!
			*
			* \param [in] KeyCode
			*
			* \retval 0 The Key is not pressed.
			* \retval 1 The Key is pressed.
			*/
			virtual int GetKeyState(int _keyCode) = 0;

			//! Get the current state of a button.
			/*!
			*
			* \param [in] ButtonCode
			*
			* \retval 0 The Button is not pressed.
			* \retval 1 The Button is pressed.
			*/
			virtual int GetButtonState(int _buttonCode) = 0;

			//! Get the change in mouse position.
			/*!
			*
			* \param [out] x a refrence to a float to store the mouse delta position x.
			* \param [out] y a refrence to a float to store the mouse delta position y.
			*
			* \retval SUCCESS no problems found.
			*/
			virtual GRETURN GetMouseDelta(float &x, float &y) = 0;

			//! Get the most recent mouse position.
			/*!
			*
			* \param [out] x a refrence to a float to store the mouse position x.
			* \param [out] y a refrence to a float to store the mouse position y.
			*
			* \retval SUCCESS no problems found.
			*/
			virtual GRETURN GetMousePosition(float &x, float &y) = 0;

			//! Get the the mouse scroll.
			/*!
			* \retval 1 The mouse is scrolling up.
			* \retval 0 The mouse is not scrolling.
			* \retval -1 The mouse is scrolling up.
			*/
			virtual float GetMouseScroll() = 0;


		};

		//! Creates a GInput Object.
		/*!
		*
		* \param [out] _outInput
		* \param [in] _data (Windows) The handle to the window (HWND).
		* \param [in] _data (Linux) Not Yet Implemented.
		* \param [in] _data (Max) Not Yet Implemented.
		*
		* \retval SUCCESS no problems found.
		* \retval FAILURE could not make an Input Object.
		* \retval INVALID_ARGUMENT _outInput and or _data is nullptr.
		*/
		GRETURN CreateGInput(GInput** _outInput, void * _data);

	};
};

#endif