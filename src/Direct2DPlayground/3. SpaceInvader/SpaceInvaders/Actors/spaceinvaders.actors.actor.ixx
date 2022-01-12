module;

#include <string>

export module spaceinvaders.actors.actor;
import spaceinvaders.actors.iactor;

export namespace SpaceInvaders::Actors
{
	class Actor : public IActor
	{
		public:
			virtual ~Actor();
			Actor(std::wstring name);

		public:
			virtual const std::wstring& GetName() const noexcept;

		protected:
			std::wstring m_name;
	};
}