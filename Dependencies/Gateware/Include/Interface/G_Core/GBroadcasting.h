#ifndef GBROADCASTING
#define GBROADCASTING

/*!
	File: GBroadcasting.h
	Purpose: A Gateware interface that can safely listen & respond to events sent from a GBroadcating interface.
	Author: Lari H. Norri
	Contributors: N/A
	Last Modified: 10/13/2016
	Copyright: 7thGate Software LLC.
	License: MIT
*/

// GBroadcasting Inherits directly from GMultiThreaded 
#include "GListener.h"

//! The core namespace to which all Gateware interfaces/structures/defines must belong
namespace GW
{
	//! The core namespace to which all Gateware fundamental interfaces must belong
	namespace CORE
	{
		//! Unique Identifier for this interface. {FF23C564-940B-44A2-9E5F-CE264659D69A}
		static const GUUIID GBroadcastingUUIID =
		{
			0xff23c564, 0x940b, 0x44a2,{ 0x9e, 0x5f, 0xce, 0x26, 0x46, 0x59, 0xd6, 0x9a }
		};

		//! The GBroadcasting Interface is capable of registering & deregistering GListener interfaces.
		//! The Broadcaster must then notifiy any listeners of advertised events via the "GListener::OnEvent" callback 
		//! During registration a GBroadcasting implmenetation may choose to support filtration via bitflags (optional) 
		class GBroadcasting : public GMultiThreaded
		{
			// All Gateware API interfaces contain no variables & are pure virtual
		public:

			//! Any listener added to this class must receive all events unless otherwise specified by the _eventMask (optional)
			//! Listeners registered to a broadcaster will have their refrence counts increased by one until deregistered
			 virtual GRETURN RegisterListener(GListener *_addListener, unsigned long long _eventMask) = 0;
			
			//! A successfully deregistered listener will no longer receive events and have it's refrence count decremented by one 
			virtual GRETURN DeregisterListener(GListener *_removeListener) = 0;
		};
	}// end CORE namespace
};// end GW namespace

#endif