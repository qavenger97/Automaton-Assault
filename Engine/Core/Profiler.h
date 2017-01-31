/*!
 * \file Profiler.h
 *
 * \author Shiyang Ao
 * \date November 2016
 *
 * 
 */
#pragma once

namespace Hourglass
{
#define MAX_PROFILE_BLOCK_COUNT 64

#define ENABLE_PROFILER 0

#if (ENABLE_PROFILER == 1)
#	define BEGIN_PROFILER()				Hourglass::g_Profiler.BeginProfiling()
#	define END_PROFILER()				Hourglass::g_Profiler.EndProfiling()
#	define BEGIN_PROFILER_BLOCK(name)	Hourglass::g_Profiler.BeginBlock(Hourglass::StrIDUtil::GetStrID(name))
#	define END_PROFILER_BLOCK(name)		Hourglass::g_Profiler.EndBlock(Hourglass::StrIDUtil::GetStrID(name))
#	define AUTO_PROFILER_BLOCK(name)	Hourglass::AutoProfilerBlock sAutoProfilerBlock(Hourglass::StrIDUtil::GetStrID(name))
#else
#	define BEGIN_PROFILER()
#	define END_PROFILER()
#	define BEGIN_PROFILER_BLOCK(name)
#	define END_PROFILER_BLOCK(name)
#	define AUTO_PROFILER_BLOCK(name)
#endif

	class Profiler
	{
	public:
		void Init();

		void BeginProfiling();
		void EndProfiling();

		// Start profiling
		void BeginBlock(StrID name);

		// Stop profiling
		void EndBlock(StrID name);

		void PrintResult();
		
	private:
		StrID		m_Names[MAX_PROFILE_BLOCK_COUNT];
		long long	m_CurrFrameTime[MAX_PROFILE_BLOCK_COUNT];
		long long	m_LastFrameTime[MAX_PROFILE_BLOCK_COUNT];

		long long	m_BlockStartTime[MAX_PROFILE_BLOCK_COUNT];
		long long	m_StartTime;
		long long	m_EndTime;
		long long	m_LastEndTime;
		UINT		m_CurrFrameCount;
		UINT		m_LastFrameCount;
		UINT		m_BlockCount;
		UINT		m_CurrBlockDepth;
		UINT		m_BlockDepth[MAX_PROFILE_BLOCK_COUNT];
		bool		m_IsProfiling;

		double		m_ClockFreq;
	};

	extern Profiler g_Profiler;

	class AutoProfilerBlock
	{
	private:
		StrID m_Name;
	public:
		AutoProfilerBlock(StrID name)
			: m_Name(name)
		{
			g_Profiler.BeginBlock(name);
		}

		~AutoProfilerBlock()
		{
			g_Profiler.EndBlock(m_Name);
		}
	};
}