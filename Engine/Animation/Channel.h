#pragma once

#include "..\Component\Transform.h"

namespace Hourglass
{
	class FileArchive;

	struct KeyFrame
	{
		Transform::TransformData m_Local;
		Transform::TransformData m_Global;

	};
	class Channel
	{
	public:

		void Serialize( FileArchive* fileArchive );

		const float* GetChannelKeyTimes() const { return m_ChannelKeyTimes.data(); }
		unsigned int GetChannelCount() const { return unsigned int(m_ChannelKeyFrames.size()); }
		const KeyFrame* GetChannelKeyFrames() const { return m_ChannelKeyFrames.data(); }
		unsigned int GetKeyCount() const { return unsigned int(m_ChannelKeyTimes.size()); }

		void AddFrame( float keyTime, const KeyFrame& keyFrame );

	private:

		std::vector<float> m_ChannelKeyTimes;
		std::vector<KeyFrame> m_ChannelKeyFrames;
	};
}