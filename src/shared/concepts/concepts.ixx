export module shared:concepts;
import std;

export namespace Shared
{
	template<typename T, typename... Args>
	concept OneOf = (std::same_as<std::remove_cvref_t<T>, Args> or ...);

	template<typename T, typename TArg>
	concept SameTypes = std::same_as<std::remove_cvref_t<T>, TArg>;
}
