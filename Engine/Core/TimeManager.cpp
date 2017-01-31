#include "pch.h"

#include "TimeManager.h"

#include <Windows.h>

namespace Hourglass
{
	TimeManager g_Time;

	void TimeManager::Init()
	{
		m_Prev = 0;
		m_Curr = 0;
		m_Start = 0;
		m_FreqInv = 0;
		m_Elapsed = 0;
		m_GameElasped = 0;
		m_Delta = 0;
		m_fixedDelta = 0.02f;
		m_timeScale = 1;
		LARGE_INTEGER freq;
		QueryPerformanceFrequency( &freq );
		m_FreqInv = 1.0 / freq.QuadPart;

		LARGE_INTEGER start;
		QueryPerformanceCounter( &start );
		m_Start = double(start.QuadPart);

		m_Curr = m_Start;
	}

	void TimeManager::Reset()
	{
		m_fixedDelta = 0.02f;
		m_timeScale = 1;
		m_GameElasped = 0;
	}

	void TimeManager::UpdateFrameTime()
	{
		// Store the previous time
		m_Prev = m_Curr;

		// Get the current end time
		LARGE_INTEGER curr;
		QueryPerformanceCounter( &curr );
		m_Curr = double(curr.QuadPart);
		m_counter = curr.QuadPart;
		// Update the delta and elapsed time
		m_unscaleDelta = float((m_Curr - m_Prev) * m_FreqInv);
		m_Elapsed += m_unscaleDelta;

		// In case a large delta time accumulation during debug, limit max delta time for one frame
		if (m_unscaleDelta > 0.05f)
			m_unscaleDelta = 0.05f;

		m_Delta = m_unscaleDelta * m_timeScale;
		m_GameElasped += m_unscaleDelta;
	}

	float TimeManager::Delta()
	{
		return m_Delta;
	}

	float TimeManager::UnscaledDelta()
	{
		return m_unscaleDelta;
	}

	float TimeManager::FixedDelta()
	{
		return m_fixedDelta;
	}

	float TimeManager::Elapsed()
	{
		return m_Elapsed;
	}

	LONGLONG TimeManager::Counter()
	{
		LARGE_INTEGER curr;
		QueryPerformanceCounter(&curr);;
		return curr.QuadPart;
	}

	float TimeManager::GameElapsed()
	{
		return m_GameElasped;
	}

	void TimeManager::SetFixedTimeStep(float step)
	{
		m_fixedDelta = step;
	}
	void TimeManager::SetTimeScale(float scale)
	{
		m_timeScale = scale;
	}
}