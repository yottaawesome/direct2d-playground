export module spaceinvaders.scenes.level1;
import spaceinvaders.scenes.scene;
import spaceinvaders.rendering.renderer;

export namespace SpaceInvaders::Scenes
{
	class Level1 : public IScene
	{
		public:
			virtual ~Level1();
			virtual void Update(const float deltaTimeSeconds) override;
			virtual void Draw(Rendering::Renderer& m_renderer) override;
			virtual void Initialise(Rendering::Renderer& renderer) override;
	};
}