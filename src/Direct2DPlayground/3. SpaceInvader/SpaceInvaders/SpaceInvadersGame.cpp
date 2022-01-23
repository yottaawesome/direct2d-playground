module;

#include <functional>
#include <Windows.h>

module spaceinvaders.spaceinvadersgame;

namespace SpaceInvaders
{
    SpaceInvadersGame::~SpaceInvadersGame() = default;

    SpaceInvadersGame::SpaceInvadersGame() {}

    void SpaceInvadersGame::Initialise()
    {
        m_mainWindow.OnResizeEvent = std::bind(&SpaceInvadersGame::OnResize, this);
        m_mainWindow.Initialise();
    }

    UINT64 SpaceInvadersGame::RunMessageLoop()
    {
        MSG msg = { 0 };
        while (msg.message != WM_QUIT)
        {
            // If there are Window messages then process them.
            if (PeekMessageW(&msg, 0, 0, 0, PM_REMOVE))
            {
                TranslateMessage(&msg);
                DispatchMessageW(&msg);
            }
            // Otherwise, do animation/game stuff.
            else
            {
                RenderScene();
            }
        }

        return msg.wParam;
    }

    void SpaceInvadersGame::RenderScene()
    {

    }

    void SpaceInvadersGame::OnResize()
    {

    }
}