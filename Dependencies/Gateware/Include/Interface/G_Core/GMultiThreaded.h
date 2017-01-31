#ifndef GMULTITHREADED
#define GMULTITHREADED

/*!
	File: GMultiThreaded.h
	Purpose: Indentifies any Gateware interface which is garunteed to be safely accessed across multiple threads.
	Author: Lari H. Norri
	Contributors: N/A
	Last Modified: 10/13/2016
	Copyright: 7thGate Software LLC.
	License: MIT
*/

// GMultiThreaded Inherits directly from GInterface 
#include "GInterface.h"

//! The core namespace to which all Gateware interfaces/structures/defines must belong
namespace GW
{
	//! The core namespace to which all Gateware fundamental interfaces must belong
	namespace CORE
	{
		//! Unique Identifier for this interface. {B0B2C21C-B7A0-42A2-BBF6-A893D66AB006}
		static const GUUIID GMultiThreadedUUIID =
		{
			0xb0b2c21c, 0xb7a0, 0x42a2,{ 0xbb, 0xf6, 0xa8, 0x93, 0xd6, 0x6a, 0xb0, 0x6 }
		};

		//! This interface is only use to label and query interfaces which promise to 100% internally support thread saftey. 
		class GMultiThreaded : public GInterface
		{
			// All Gateware API interfaces contain no variables & are pure virtual
		public:
			// This class contains no unique routines that must be overridden
			// However, unlike GSingleThreaded all overridden and downstream operations MUST be thread safe
		};
	}// end CORE namespace
};// end GW namespace

#endif