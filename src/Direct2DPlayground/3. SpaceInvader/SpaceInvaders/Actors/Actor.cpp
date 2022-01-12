module;

#include <string>

module spaceinvaders.actors.actor;

namespace SpaceInvaders::Actors
{
	Actor::~Actor() = default;

	Actor::Actor(std::wstring name) : m_name(std::move(name)) {}

	const std::wstring& Actor::GetName() const noexcept
	{
		return m_name;
	}
}