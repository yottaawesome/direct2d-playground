export module spaceinvaders.scenes.scene;
import spaceinvaders.rendering.renderer;

export namespace SpaceInvaders::Scenes
{
	struct IScene
	{
		virtual ~IScene();
		virtual void Update(const float deltaTimeSeconds) = 0;
		virtual void Draw(Rendering::Renderer& renderer) = 0;
		virtual void Initialise(Rendering::Renderer& renderer) = 0;
	};
}