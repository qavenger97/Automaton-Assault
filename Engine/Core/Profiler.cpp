/*!
 * \file Profiler.cpp
 *
 * \author Shiyang Ao
 * \date November 2016
 *
 * 
 */

#include "pch.h"

#include "Profiler.h"
#include "Renderer\DevTextRenderer.h"

Hourglass::Profiler Hourglass::g_Profiler;
const float kProfilerSampleInterval = 200.0f;

void Hourglass::Profiler::Init()
{
	ZeroMemory(m_CurrFrameTime, sizeof(m_CurrFrameTime));
	ZeroMemory(m_LastFrameTime, sizeof(m_LastFrameTime));

	m_BlockCount = 0;

	LARGE_INTEGER li;
	if (!QueryPerformanceFrequency(&li))
		return;

	m_ClockFreq = double(li.QuadPart) / 1000.0;
	m_StartTime = li.QuadPart;
	m_CurrFrameCount = 0;
	m_LastFrameCount = 0;
	m_CurrBlockDepth = 0;
	m_IsProfiling = false;
}

void Hourglass::Profiler::BeginProfiling()
{
	m_IsProfiling = true;
}

void Hourglass::Profiler::EndProfiling()
{
	m_CurrFrameCount++;

	LARGE_INTEGER li;
	QueryPerformanceCounter(&li);

	m_EndTime = li.QuadPart - m_StartTime;
	if (m_EndTime > m_ClockFreq * kProfilerSampleInterval)
	{
		// Copy time for current frame to last frame
		memcpy_s(m_LastFrameTime, sizeof(m_LastFrameTime), m_CurrFrameTime, sizeof(m_CurrFrameTime));
		ZeroMemory(m_CurrFrameTime, sizeof(m_CurrFrameTime));

		LARGE_INTEGER li;
		QueryPerformanceCounter(&li);
		m_StartTime = li.QuadPart;
		m_LastFrameCount = m_CurrFrameCount;
		m_CurrFrameCount = 0;
		m_LastEndTime = m_EndTime;
	}

	m_IsProfiling = false;
}

void Hourglass::Profiler::BeginBlock(StrID name)
{
	if (!m_IsProfiling)
		return;

	m_CurrBlockDepth++;

	UINT i = 0;
	for (; i < m_BlockCount; i++)
	{
		if (m_Names[i] == name)
		{
			break;
		}
	}

	if (i == m_BlockCount)
	{
		assert(m_BlockCount < MAX_PROFILE_BLOCK_COUNT && "Running out of profiler block count");

		m_Names[m_BlockCount++] = name;
	}

	LARGE_INTEGER li;
	QueryPerformanceCounter(&li);
	m_BlockStartTime[i] = li.QuadPart;
	m_BlockDepth[i] = m_CurrBlockDepth;
}

void Hourglass::Profiler::EndBlock(StrID name)
{
	if (!m_IsProfiling)
		return;

	UINT i = 0;
	for (; i < m_BlockCount; i++)
	{
		if (m_Names[i] == name)
		{
			break;
		}
	}
	
	assert(i < m_BlockCount && "Incorrect calling of EndBlock since BeginBlock was not called on this name");

	LARGE_INTEGER li;
	QueryPerformanceCounter(&li);
	m_CurrFrameTime[i] += li.QuadPart - m_BlockStartTime[i];
	m_CurrBlockDepth--;
}

void Hourglass::Profiler::PrintResult()
{
	// Avoid divide by zero issue
	if (m_LastFrameCount == 0)
		m_LastFrameCount = 1;

	double e = (double)m_LastEndTime / (m_ClockFreq * m_LastFrameCount);
	UINT i = 0;
	char buf[1024];

	for (; i < m_BlockCount; i++)
	{
		double t = (double)m_LastFrameTime[i] / (m_ClockFreq * m_LastFrameCount);
		int offset = (m_BlockDepth[i] - 1) * 4;
		DevTextRenderer::DrawText(StrIDUtil::GetStringFromStrID(m_Names[i]), 5 + offset, i + 5);
		sprintf_s(buf, "%lf ms", t);
		DevTextRenderer::DrawText(buf, 30 + offset, i + 5);
		sprintf_s(buf, "%4.2lf%%", t / e * 100);
		DevTextRenderer::DrawText(buf, 50 + offset, i + 5);
	}

	i++;
	DevTextRenderer::DrawText("Total: ", 5, i + 5);
	sprintf_s(buf, "%lf ms", e);
	DevTextRenderer::DrawText(buf, 30, i + 5);
}
