export module core:time_gametimer;
import std;
import :win32;

export namespace Time
{
	struct GameTimer
	{
		// Based on Frank Luna's GameTimer class from Intro to 3D Game Programming with DirectX 12
		GameTimer()
		{
			size_t countsPerSec;
			// https://docs.microsoft.com/en-us/windows/win32/api/profileapi/nf-profileapi-queryperformancefrequency
			Win32::QueryPerformanceFrequency(reinterpret_cast<Win32::LARGE_INTEGER*>(&countsPerSec));
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
			std::int64_t currentTime;
			// https://docs.microsoft.com/en-us/windows/win32/api/profileapi/nf-profileapi-queryperformancecounter
			Win32::QueryPerformanceCounter((Win32::LARGE_INTEGER*)&currentTime);

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

			Win32::QueryPerformanceCounter((Win32::LARGE_INTEGER*)&m_stopTime);
			m_isStopped = true;
		}

		void Reset() noexcept
		{
			std::int64_t currentTime;
			Win32::QueryPerformanceCounter((Win32::LARGE_INTEGER*)&currentTime);

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

			Win32::QueryPerformanceCounter((Win32::LARGE_INTEGER*)&m_currentTime);

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
		double m_secondsPerCount = 0;
		double m_deltaTime = 0;
		std::int64_t m_baseTime = 0;
		std::int64_t m_currentTime = 0;
		std::int64_t m_totalPausedTime = 0;
		std::int64_t m_stopTime = 0;
		std::int64_t m_previousTime = 0;
		bool m_isStopped = false;
	};
}