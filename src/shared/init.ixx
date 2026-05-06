export module shared:init;

export namespace Shared
{
	struct InitTag {} constexpr Init{};
	struct NoInitTag {} constexpr NoInit{};
}
