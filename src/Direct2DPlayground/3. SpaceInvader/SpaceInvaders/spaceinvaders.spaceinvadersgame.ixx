module;

#include <Windows.h>

export module spaceinvaders.spaceinvadersgame;
import spaceinvaders.mainwindow;

export namespace SpaceInvaders
{
	class SpaceInvadersGame
	{
		public:
			virtual ~SpaceInvadersGame();
			SpaceInvadersGame();

		public:
			virtual void Initialise();
			virtual UINT64 RunMessageLoop();
			virtual void OnResize();

		protected:
			virtual void RenderScene();

		protected:
			MainWindow m_mainWindow;
	};
}