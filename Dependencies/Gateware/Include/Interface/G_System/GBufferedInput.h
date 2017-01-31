#ifndef _GBUFFEREDINPUT_H
#define _GBUFFEREDINPUT_H

#include "../G_Core/GBroadcasting.h"
#include "../G_System/GKeyDefines.h"

namespace GW
{
	namespace CORE
	{

#pragma pack(push, 1)
		//! G_INPUT_DATA will hold any information you may need about an Input Event.
		/*!
		*/
		struct G_INPUT_DATA
		{
			int _data;				/*<_data Data store the key/button information. */ 
			int _x;					/*<_x Window Mouse position x when event is send. */
			int _y;					/*<_y Window Mouse position y when event is send. */
			int _screenX;			/*<_x Screen Mouse position x when event is send. */
			int _screenY;			/*<_y Screen Mouse position y when event is send. */
		};
#pragma pack(pop)

		//! GBInput_Events hold the possible events that can be sent from GBufferedInput.
		/*!
		*/
		enum GBInput_Events {
			KEYPRESSED,				/*<KEYPRESSED Key pressed event. */ 
			KEYRELEASED,			/*<KEYRELEASED Key released event. */ 
			BUTTONPRESSED,			/*<BUTTONPRESSED Button pressed event. */ 
			BUTTONRELEASED,			/*<BUTTONRELEASED Button released event. */ 
			MOUSESCROLL,			/*<MOUSESCROLL Mouse scroll event. */ 
		};

		static const GUUIID GBufferedInputUUIID =
		{
			0x4cba9d69, 0x1b32, 0x43da,{ 0xb7, 0xb2, 0xa4, 0x21, 0xc5, 0x78, 0x18, 0xf0 }
		};

		//! A Multi threaded buffered input library.
		/*!
		* Register with a GBufferedInput to recieve mouse and keyboard events.
		*/
		class GBufferedInput : public GBroadcasting { };

		//! Creates a GBufferedInput Object.
		/*!
		*
		* \param [out] _outBufferedInput
		* \param [in] _data (Windows) The handle to the window (HWND).
		* \param [in] _data (Linux) Not Yet Implemented.
		* \param [in] _data (Max) Not Yet Implemented.
		*
		* \retval SUCCESS no problems found.
		* \retval FAILURE could not make an BufferedInput Object.
		* \retval INVALID_ARGUMENT _outInput and or _data is nullptr.
		*/
		GRETURN CreateGBufferedInput(GBufferedInput** _outBufferedInput, void * _data);
	};
};
#endif