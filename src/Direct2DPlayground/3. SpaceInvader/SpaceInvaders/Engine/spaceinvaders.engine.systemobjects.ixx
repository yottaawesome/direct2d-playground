export module spaceinvaders.engine.systemobjects;
import spaceinvaders.engine.renderer;
import spaceinvaders.engine.d2dfactory;
import spaceinvaders.ui;
import core.time.gametimer;

export namespace SpaceInvaders::Engine
{
	struct SystemObjects
	{
		Renderer Renderer;
		D2DFactory Factory;
		UI::MainWindow Window;
		Core::Time::GameTimer Timer;
	};
}