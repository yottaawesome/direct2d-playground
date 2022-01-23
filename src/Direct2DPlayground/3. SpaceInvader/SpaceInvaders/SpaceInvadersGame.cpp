module;

#include <Windows.h>

module spaceinvaders.spaceinvadersgame;

namespace SpaceInvaders
{
	SpaceInvadersGame::~SpaceInvadersGame() = default;
	SpaceInvadersGame::SpaceInvadersGame() {}

	// The windows procedure.
	LRESULT CALLBACK SpaceInvadersGame::AppProc(
		HWND hWnd,
		UINT message,
		WPARAM wParam,
		LPARAM lParam
	)
	{
		return 0;
	};
}