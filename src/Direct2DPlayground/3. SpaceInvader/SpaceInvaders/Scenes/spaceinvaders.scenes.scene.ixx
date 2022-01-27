export module spaceinvaders.scenes.scene;

export namespace SpaceInvaders::Scenes
{
	struct Scene
	{
		virtual ~Scene();
		virtual void Update(const float deltaTimeSeconds) = 0;
		virtual void Draw() = 0;
	};
}