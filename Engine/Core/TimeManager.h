#pragma once

namespace Hourglass
{
	class TimeManager
	{
	public:

		void Init();

		void Reset();

		void UpdateFrameTime();

		float Delta();
		float UnscaledDelta();
		float FixedDelta();
		float Elapsed();
		LONGLONG Counter();
		float GameElapsed();
		void SetFixedTimeStep(float step);
		void SetTimeScale(float scale);
		inline float GetTimeScale()const { return m_timeScale; }
	private:

		double m_Prev;
		double m_Curr;
		double m_Start;
		double m_FreqInv;

		float m_Elapsed;
		float m_GameElasped;
		float m_Delta;
		float m_unscaleDelta;
		float m_fixedDelta;
		float m_timeScale;

		LONGLONG m_counter;
	};

	extern TimeManager g_Time;
}