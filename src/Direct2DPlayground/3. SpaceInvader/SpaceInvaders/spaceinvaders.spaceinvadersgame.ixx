module;

#include <Windows.h>

export module spaceinvaders.spaceinvadersgame;
import core.time.gametimer;
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
			// Adapted from Frank D. Luna's D3D12 samples.
			virtual void CalculateFrameStats();

		protected:
			virtual void RenderScene();

		protected:
			UI::MainWindow m_mainWindow;
			Rendering::Renderer m_renderer;
			Core::Time::GameTimer m_timer;
			size_t m_colourIndex;
			float m_timeElapsed;
	};
}