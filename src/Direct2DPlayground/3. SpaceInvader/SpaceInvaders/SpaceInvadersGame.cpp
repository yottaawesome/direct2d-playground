module;

#include <functional>
#include <Windows.h>
#include <D2d1helper.h>

module spaceinvaders.spaceinvadersgame;
import core.error.comerror;

namespace SpaceInvaders
{
    SpaceInvadersGame::~SpaceInvadersGame() = default;

    SpaceInvadersGame::SpaceInvadersGame() {}

    void SpaceInvadersGame::Initialise()
    {
        m_mainWindow.OnResizeEvent = std::bind(&SpaceInvadersGame::OnResize, this);
        m_mainWindow.Initialise();
        m_renderer.BindRenderTarget(
            m_mainWindow.GetHandle(), 
            m_mainWindow.GetClientWidth(), 
            m_mainWindow.GetClientHeight()
        );
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

    void SpaceInvadersGame::RenderScene() try
    {
        m_renderer.Draw(
            [this]()
            {
                m_renderer.Clear(D2D1::ColorF(D2D1::ColorF::Aquamarine));
            }
        );
    }
    catch (const Core::Error::COMError& ex)
    {
        if (ex.GetErrorCode() != D2DERR_RECREATE_TARGET)
            throw;
        // can fail due to D2DERR_RECREATE_TARGET
        m_renderer.BindRenderTarget(
            m_mainWindow.GetHandle(),
            m_mainWindow.GetClientWidth(),
            m_mainWindow.GetClientHeight()
        );
        // will also need to recreate the device-specific resources
    }

    void SpaceInvadersGame::OnResize()
    {
        m_renderer.Resize(m_mainWindow.GetClientWidth(), m_mainWindow.GetClientHeight());
    }
}