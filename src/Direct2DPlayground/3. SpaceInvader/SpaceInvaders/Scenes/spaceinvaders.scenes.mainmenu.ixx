export module spaceinvaders.scenes.mainmenu;
import spaceinvaders.scenes.scene;

export namespace SpaceInvaders::Scenes
{
	class MainMenu : public Scene
	{
		public:
			virtual ~MainMenu();
			virtual void Update(const float deltaTimeSeconds) override;
			virtual void Draw() override;
	};
}