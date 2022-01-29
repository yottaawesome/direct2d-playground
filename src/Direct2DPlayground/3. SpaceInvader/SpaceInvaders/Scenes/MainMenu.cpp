module;

#include <functional>
#include <d2d1.h>
#include <d2d1helper.h>

module spaceinvaders.scenes.mainmenu;

namespace SpaceInvaders::Scenes
{
	MainMenu::~MainMenu() { }
    
    MainMenu::MainMenu() : m_delta(0), m_totalTime(0), m_colourIndex(0) {}

	void MainMenu::Update(const float deltaTimeSeconds)
	{
        m_delta = deltaTimeSeconds;
        m_totalTime += deltaTimeSeconds;
	}

	void MainMenu::Draw(Engine::Renderer& renderer)
	{
        renderer.Draw(
            [this, &renderer]()
            {
                if (m_totalTime >= 1.f)
                {
                    m_colourIndex++;
                    m_totalTime = 0;
                }

                switch (m_colourIndex)
                {
                    case 1:
                        renderer.Clear(D2D1::ColorF(D2D1::ColorF::Azure));
                        break;

                    case 2:
                        renderer.Clear(D2D1::ColorF(D2D1::ColorF::Bisque));
                        break;

                    default:
                        renderer.Clear(D2D1::ColorF(D2D1::ColorF::Aquamarine));
                        if (m_colourIndex > 2)
                            m_colourIndex = 0;
                }

                renderer.GetRaw()->DrawBitmap(m_bitmap.Get());
            }
        );
	}

    void MainMenu::Initialise(Engine::Renderer& renderer)
    {
        m_bitmap = renderer.LoadBitmap(L"PlayerShipBlue.png");
    }
}