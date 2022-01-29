export module spaceinvaders.scenes.scene;
import spaceinvaders.engine.renderer;
import spaceinvaders.engine.d2dfactory;

export namespace SpaceInvaders::Scenes
{
	struct IScene
	{
		virtual ~IScene();
		virtual void Update(const float deltaTimeSeconds) = 0;
		virtual void Draw(Engine::Renderer& renderer) = 0;
		virtual void Initialise(Engine::Renderer& renderer) = 0;
	};
}