#pragma once

namespace Hourglass
{
	namespace MessageType
	{
		enum
		{
			// Engine messages
			kCollision,
			kOnTriggerEnter,
			kOnTriggerExit,
			kOnEvent,

			kResetLevel,
			kToggleVsync,
			kToggleBloom,

			kEngineMessageCount,		// Note: game message enumeration should start with value equals to kEngineMessageCount
		};
	}

	class Message
	{
	public:
		virtual int GetType() const
		{
			// Get message type should always be override
			assert(0);
			return -1;
		}
	};
}