#include "pch.h"
#include "Clip.h"
#include "Core\FileArchive.h"

void Hourglass::Clip::Serialize( FileArchive* archive )
{
	archive->EnsureHeader( "HANI", 4 );
	archive->Serialize( (unsigned int)m_ChannelCount );

	for (unsigned int i = 0; i < m_ChannelCount; ++i)
	{
		m_Channels[i].Serialize( archive );
	}

	archive->Serialize( m_Name );
}
