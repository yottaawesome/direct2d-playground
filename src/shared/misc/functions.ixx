export module shared:misc.functions;
import std;

export namespace Shared
{
	// This is a helper function to create a vector of a given size with default-initialised elements.
	// This is useful because it avoids the initialiser list constructor of std::vector, which 
	// would create the wrong number of elements.
	template<typename T, size_t N>
	constexpr auto MakeVector() -> std::vector<T>
	{
		auto vec = std::vector<T>{};
		vec.resize(N);
		return vec;
	}
}
