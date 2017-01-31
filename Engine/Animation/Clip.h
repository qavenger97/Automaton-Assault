#pragma once

#include "Channel.h"

namespace Hourglass
{
	class FileArchive;

	class Clip
	{
	public:

		void Serialize( FileArchive* archive );

		unsigned int GetChannelCount() const { return m_ChannelCount; }
		Channel* GetChannels() { return m_Channels; }
		const char* GetName() const { return m_Name.c_str(); }
		const char* GetPath() const { return m_Path.c_str(); }

		void AddChannel( const Channel& channel ) { m_Channels[m_ChannelCount++] = channel; }
		void SetName( const char* name ) { m_Name = name; }	
		void SetPath( const char* path ) { m_Path = path; }
		void SetChannelCount( unsigned int count ) { m_ChannelCount = count; }

	private:

		static const uint32_t kMaxJoints = 96;
		Channel m_Channels[kMaxJoints];
		uint32_t m_ChannelCount = 0;
		std::string m_Name;
		std::string m_Path;
	};
}