module;

#include <Windows.h>

export module spaceinvaders.spaceinvadersgame;

export namespace SpaceInvaders
{
	class SpaceInvadersGame
	{
		public:
			virtual ~SpaceInvadersGame();
			SpaceInvadersGame();

		public:

		protected:
			// The windows procedure.
			static LRESULT CALLBACK AppProc(
				HWND hWnd,
				UINT message,
				WPARAM wParam,
				LPARAM lParam
			);

		protected:
	};
}