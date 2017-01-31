#include "pch.h"
#include "Channel.h"
#include "Core\FileArchive.h"

void Hourglass::Channel::Serialize( FileArchive* archive )
{
	archive->Serialize( m_ChannelKeyFrames );
	archive->Serialize( m_ChannelKeyTimes );
}

void Hourglass::Channel::AddFrame( float keyTime, const KeyFrame& keyFrame )
{
	m_ChannelKeyTimes.push_back( keyTime );
	m_ChannelKeyFrames.push_back( keyFrame );
}