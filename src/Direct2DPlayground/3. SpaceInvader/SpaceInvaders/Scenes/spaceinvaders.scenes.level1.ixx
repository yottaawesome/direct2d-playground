export module spaceinvaders.scenes.level1;
import spaceinvaders.scenes.scene;

export namespace SpaceInvaders::Scenes
{
	class Level1 : public Scene
	{
		public:
			virtual ~Level1();
			virtual void Update(const float deltaTimeSeconds) override;
			virtual void Draw() override;
	};
}