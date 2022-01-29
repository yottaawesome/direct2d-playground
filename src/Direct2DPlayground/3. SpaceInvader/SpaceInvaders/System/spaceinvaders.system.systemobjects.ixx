export module spaceinvaders.system.systemobjects;
import spaceinvaders.rendering.renderer;
import spaceinvaders.ui;
import core.time.gametimer;

export namespace SpaceInvaders::System
{
	struct SystemObjects
	{
		Rendering::Renderer Renderer;
		UI::MainWindow Window;
		Core::Time::GameTimer Timer;
	};
}