module;

#include <Windows.h>

export module core:time_gametimer;
import std;

export namespace Time
{
	struct GameTimer
	{
		// Based on Frank Luna's GameTimer class from Intro to 3D Game Programming with DirectX 12
		GameTimer()
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

		float TotalTime() const noexcept
		{
			if (m_isStopped)
				return (float)(((m_stopTime - m_totalPausedTime) - m_baseTime) * m_secondsPerCount);
			return (float)(((m_currentTime - m_totalPausedTime) - m_baseTime) * m_secondsPerCount);
		}

		float DeltaTime() const noexcept
		{
			return (float)m_deltaTime;
		}

		void Start() noexcept
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

		void Stop() noexcept
		{
			if (m_isStopped)
				return;

			QueryPerformanceCounter((LARGE_INTEGER*)&m_stopTime);
			m_isStopped = true;
		}

		void Reset() noexcept
		{
			__int64 currentTime;
			QueryPerformanceCounter((LARGE_INTEGER*)&currentTime);

			m_baseTime = currentTime;
			m_previousTime = currentTime;
			m_stopTime = 0;
			m_isStopped = false;
		}

		void Tick() noexcept
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

	protected:
		double m_secondsPerCount;
		double m_deltaTime;
		std::int64_t m_baseTime;
		std::int64_t m_currentTime;
		std::int64_t m_totalPausedTime;
		std::int64_t m_stopTime;
		std::int64_t m_previousTime;
		bool m_isStopped;
	};
}