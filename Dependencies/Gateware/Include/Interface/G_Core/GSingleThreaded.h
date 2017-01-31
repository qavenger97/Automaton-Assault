#ifndef GSINGLETHREADED
#define GSINGLETHREADED

/*!
	File: GSingleThreaded.h
	Purpose: Indentifies any Gateware interface which may NOT be safely accessed across multiple threads.
	Author: Lari H. Norri
	Contributors: N/A
	Last Modified: 10/13/2016
	Copyright: 7thGate Software LLC.
	License: MIT
*/

// GSingleThreaded Inherits directly from GInterface 
#include "GInterface.h"

//! The core namespace to which all Gateware interfaces/structures/defines must belong
namespace GW
{
	//! The core namespace to which all Gateware fundamental interfaces must belong
	namespace CORE
	{
		//! Unique Identifier for this interface. {088E5BC2-8BF4-4972-9537-FACBFA142789}
		static const GUUIID GSingleThreadedUUIID =
		{
			0x88e5bc2, 0x8bf4, 0x4972,{ 0x95, 0x37, 0xfa, 0xcb, 0xfa, 0x14, 0x27, 0x89 }
		};

		//! This interface is only use to label and query interfaces which are not designed internally to support thread saftey. 
		class GSingleThreaded : public GInterface
		{
			// All Gateware API interfaces contain no variables & are pure virtual
		public:
			// This class contains no unique routines that must be overridden
		};
	}// end CORE namespace
};// end GW namespace

#endif