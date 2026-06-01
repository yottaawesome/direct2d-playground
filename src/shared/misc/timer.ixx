export module shared:misc.timer;
import std;

export namespace Shared
{
	class Timer
	{
	public:
		auto Advance(this auto&& self) -> float
		{
			const auto now = std::chrono::steady_clock::now();
			auto dt = std::chrono::duration<float>(now - self.last).count(); // seconds (can be decimal)
			self.last = now;
			return dt;
		}

		auto AdvanceMs(this auto&& self) -> std::chrono::milliseconds
		{
			const auto now = std::chrono::steady_clock::now();
			auto dt = std::chrono::duration_cast<std::chrono::milliseconds>(now - self.last);
			self.last = now;
			return dt;
		}

		auto Reset(this auto&& self)
		{
			self.last = std::chrono::steady_clock::now();
		}

		auto GetLast(this auto&& self) -> std::chrono::steady_clock::time_point
		{
			return self.last;
		}
	protected:
		std::chrono::steady_clock::time_point last = std::chrono::steady_clock::now();
	};
}
