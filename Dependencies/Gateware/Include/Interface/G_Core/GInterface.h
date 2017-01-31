#ifndef GINTERFACE
#define GINTERFACE
/*!
	File: GInterface.h
	Purpose: The fundamental interface which all Gateware interfaces must adhere to at a minimum.
	Author: Lari H. Norri
	Contributors: N/A
	Last Modified: 9/29/2016
	Copyright: 7thGate Software LLC.
	License: MIT
*/

// contains all defined elements shared among base interfaces
#include "GDefines.h"

//! The core namespace to which all Gateware interfaces/structures/defines must belong
namespace GW
{
	//! The core namespace to which all Gateware fundamental interfaces must belong
	namespace CORE
	{
		//! Unique Identifier for this interface. {F73C8B73-7C68-4FEE-BF82-83C267277313} 
		static const GUUIID GInterfaceUUIID =
		{
			0xf73c8b73, 0x7c68, 0x4fee,{ 0xbf, 0x82, 0x83, 0xc2, 0x67, 0x27, 0x73, 0x13 }
		};

		//! Base interface all Gateware interfaces must support at a minimum.
		//! Core features include: Interface Upgrades, Refrence Counting, Event Broadcasting 
		class GInterface
		{
			// All Gateware API interfaces contain no variables & are pure virtual
		public:
			//! Return the total number of active refrences to this object
			virtual GRETURN GetCount(unsigned int &_outCount) = 0;
			
			//! Increase the total number of active refrences to this object
			//! End users should only call this operation if they are familiar with reference counting behavior
			virtual GRETURN IncrementCount() = 0;

			//! Decrease the total number of active refrences to this object
			//! Once the internal count reaches zero this object will be deallocated and your pointer will become invalid
			virtual GRETURN DecrementCount() = 0;

			//! Requests an interface that may or may not be supported by this object 
			virtual GRETURN RequestInterface(const GUUIID &_interfaceID, void** _outputInterface) = 0;
		};
	}// end CORE namespace
};// end GW namespace

#endif