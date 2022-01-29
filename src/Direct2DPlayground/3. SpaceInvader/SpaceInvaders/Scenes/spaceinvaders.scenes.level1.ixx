export module spaceinvaders.scenes.level1;
import spaceinvaders.scenes.scene;
import spaceinvaders.engine.renderer;
import spaceinvaders.engine.d2dfactory;

export namespace SpaceInvaders::Scenes
{
	class Level1 : public IScene
	{
		public:
			virtual ~Level1();
			virtual void Update(const float deltaTimeSeconds) override;
			virtual void Draw(Engine::Renderer& m_renderer) override;
			virtual void Initialise(Engine::Renderer& renderer) override;
	};
}