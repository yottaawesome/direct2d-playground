export module shared:timer;
import std;

export namespace Shared
{
	class Timer
	{
	public:
		std::chrono::steady_clock::time_point last = std::chrono::steady_clock::now();

		auto Advance(this auto&& self) -> float
		{
			const auto now = std::chrono::steady_clock::now();
			auto dt = std::chrono::duration<float>(now - self.last).count(); // seconds (can be decimal)
			self.last = now;
			return dt;
		}
	};
}