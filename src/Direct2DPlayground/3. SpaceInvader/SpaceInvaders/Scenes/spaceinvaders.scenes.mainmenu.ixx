module;

#include <d2d1.h>
#include <d2d1helper.h>
#include <wrl/client.h>

export module spaceinvaders.scenes.mainmenu;
import spaceinvaders.scenes.scene;

export namespace SpaceInvaders::Scenes
{
	class MainMenu : public IScene
	{
		public:
			virtual ~MainMenu();
			MainMenu();
			virtual void Update(const float deltaTimeSeconds) override;
			virtual void Draw(Rendering::Renderer& renderer) override;
			virtual void Initialise(Rendering::Renderer& renderer) override;

		protected:
			float m_delta;
			float m_totalTime;
			int m_colourIndex;
			Microsoft::WRL::ComPtr<ID2D1Bitmap> m_bitmap;
	};
}