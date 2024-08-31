module;

#include <Windows.h>

module core.time.gametimer;

namespace Core::Time
{
	GameTimer::~GameTimer() {}

	GameTimer::GameTimer()
		: m_secondsPerCount(0),
		m_deltaTime(0),
		m_baseTime(0),
		m_currentTime(0),
		m_totalPausedTime(0),
		m_stopTime(0),
		m_previousTime(0),
		m_isStopped(false)
	{
		size_t countsPerSec;
		// https://docs.microsoft.com/en-us/windows/win32/api/profileapi/nf-profileapi-queryperformancefrequency
		QueryPerformanceFrequency(reinterpret_cast<LARGE_INTEGER*>(&countsPerSec));
		m_secondsPerCount = 1.0 / (double)countsPerSec;
	}

	float GameTimer::TotalTime() const noexcept
	{
		if (m_isStopped)
			return (float)(((m_stopTime - m_totalPausedTime) - m_baseTime) * m_secondsPerCount);
		return (float)(((m_currentTime - m_totalPausedTime) - m_baseTime) * m_secondsPerCount);
	}

	float GameTimer::DeltaTime() const noexcept
	{
		return (float)m_deltaTime;
	}

	void GameTimer::Start() noexcept
	{
		__int64 currentTime;
		// https://docs.microsoft.com/en-us/windows/win32/api/profileapi/nf-profileapi-queryperformancecounter
		QueryPerformanceCounter((LARGE_INTEGER*)&currentTime);

		if (m_isStopped == false)
			return;

		m_totalPausedTime += (currentTime - m_stopTime);
		m_isStopped = false;
		m_previousTime = currentTime;
		m_stopTime = 0;
	}

	void GameTimer::Stop() noexcept
	{
		if (m_isStopped)
			return;

		QueryPerformanceCounter((LARGE_INTEGER*)&m_stopTime);
		m_isStopped = true;
	}

	void GameTimer::Reset() noexcept
	{
		__int64 currentTime;
		QueryPerformanceCounter((LARGE_INTEGER*)&currentTime);

		m_baseTime = currentTime;
		m_previousTime = currentTime;
		m_stopTime = 0;
		m_isStopped = false;
	}

	void GameTimer::Tick() noexcept
	{
		if (m_isStopped)
		{
			m_deltaTime = 0.0;
			return;
		}

		QueryPerformanceCounter((LARGE_INTEGER*)&m_currentTime);

		// Time difference between this frame and the previous.
		m_deltaTime = (m_currentTime - m_previousTime) * m_secondsPerCount;

		// Prepare for next frame.
		m_previousTime = m_currentTime;

		// Force nonnegative.  The DXSDK's CDXUTTimer mentions that if the 
		// processor goes into a power save mode or we get shuffled to another
		// processor, then m_deltaTime can be negative.
		if (m_deltaTime < 0.0)
			m_deltaTime = 0.0;
	}
}