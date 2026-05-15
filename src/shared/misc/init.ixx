export module shared:misc.init;

export namespace Shared
{
	struct InitTag {} constexpr Init{};
	struct NoInitTag {} constexpr NoInit{};
}
