export module shared:misc.init;

export namespace Shared
{
	struct InitTag
	{ 
		constexpr auto operator==(const InitTag&) const noexcept { return true; } 
	} constexpr Init;
	struct NoInitTag
	{ 
		constexpr auto operator==(const NoInitTag&) const noexcept { return true; } 
	} constexpr NoInit;
}
