module;

#include <string>

export module spaceinvaders.actors.iactor;

export namespace SpaceInvaders::Actors
{
	struct IActor
	{
		virtual ~IActor();
		virtual const std::wstring& GetName() const noexcept = 0;
	};
}