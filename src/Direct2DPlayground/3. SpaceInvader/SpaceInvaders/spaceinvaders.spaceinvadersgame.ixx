module;

#include <Windows.h>

export module spaceinvaders.spaceinvadersgame;
import spaceinvaders.ui;
import spaceinvaders.rendering.renderer;

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
			virtual void OnResize(const unsigned width, const unsigned height);
			virtual void OnInputPressed(const UI::InputType type, const wchar_t key);

		protected:
			virtual void RenderScene();

		protected:
			UI::MainWindow m_mainWindow;
			Rendering::Renderer m_renderer;
	};
}