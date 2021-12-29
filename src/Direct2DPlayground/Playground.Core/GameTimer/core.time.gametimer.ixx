export module core.time.gametimer;

export namespace Core::Time
{
	class GameTimer
	{
		// Based on Frank Luna's GameTimer class from Intro to 3D Game Programming with DirectX 12
		public:
			virtual ~GameTimer();
			GameTimer();

		public:
			virtual float TotalTime() const noexcept; // in seconds
			virtual float DeltaTime() const noexcept; // in seconds

			virtual void Start() noexcept;
			virtual void Stop() noexcept;
			virtual void Reset() noexcept;
			virtual void Tick() noexcept;

		protected:
			double m_secondsPerCount;
			double m_deltaTime;
			__int64 m_baseTime;
			__int64 m_currentTime;
			__int64 m_totalPausedTime;
			__int64 m_stopTime;
			__int64 m_previousTime;
			bool m_isStopped;
	};
}