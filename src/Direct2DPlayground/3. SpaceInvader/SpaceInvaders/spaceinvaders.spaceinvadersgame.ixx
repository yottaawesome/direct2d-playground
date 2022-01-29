module;

#include <memory>
#include <vector>
#include <Windows.h>

export module spaceinvaders.spaceinvadersgame;
import core.time.gametimer;
import spaceinvaders.ui;
import spaceinvaders.engine.renderer;
import spaceinvaders.scenes;
import spaceinvaders.engine.systemobjects;

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

		protected:
			Engine::SystemObjects m_system;
			size_t m_colourIndex;
			float m_timeElapsed;
			std::shared_ptr<SpaceInvaders::Scenes::IScene> m_currentScene;
			std::vector<std::shared_ptr<SpaceInvaders::Scenes::IScene>> m_scenes;
	};
}