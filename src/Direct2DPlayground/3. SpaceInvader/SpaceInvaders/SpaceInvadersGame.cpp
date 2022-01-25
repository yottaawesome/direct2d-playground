module;

#include <iostream>
#include <string>
#include <format>
#include <functional>
#include <Windows.h>
#include <D2d1helper.h>

module spaceinvaders.spaceinvadersgame;
import core.error.comerror;

namespace SpaceInvaders
{
    SpaceInvadersGame::~SpaceInvadersGame() = default;

    SpaceInvadersGame::SpaceInvadersGame() 
        : m_colourIndex(0)
        , m_timeElapsed(0)
    {}

    void SpaceInvadersGame::Initialise()
    {
        m_mainWindow.OnResize = std::bind(&SpaceInvadersGame::OnResize, this, std::placeholders::_1, std::placeholders::_2);
        m_mainWindow.OnInputPressed = std::bind(&SpaceInvadersGame::OnInputPressed, this, std::placeholders::_1, std::placeholders::_2);
        m_mainWindow.Initialise();
        m_renderer.BindRenderTarget(
            m_mainWindow.GetHandle(), 
            m_mainWindow.GetClientWidth(), 
            m_mainWindow.GetClientHeight()
        );
    }

    UINT64 SpaceInvadersGame::RunMessageLoop()
    {
        m_timer.Reset();

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
                CalculateFrameStats();
                m_timer.Tick();
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
                m_timeElapsed += m_timer.DeltaTime();
                if (m_timeElapsed >= 1.f)
                {
                    m_colourIndex++;
                    m_timeElapsed = 0;
                }
                
                switch (m_colourIndex)
                {
                    case 1:
                        m_renderer.Clear(D2D1::ColorF(D2D1::ColorF::Azure));
                        break;

                    case 2:
                        m_renderer.Clear(D2D1::ColorF(D2D1::ColorF::Bisque));
                        break;

                    default:
                        m_renderer.Clear(D2D1::ColorF(D2D1::ColorF::Aquamarine));
                        if (m_colourIndex > 2)
                            m_colourIndex = 0;
                }

                //m_renderer.Clear(colour);
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

    void SpaceInvadersGame::OnResize(const unsigned width, const unsigned height)
    {
        m_renderer.Resize(m_mainWindow.GetClientWidth(), m_mainWindow.GetClientHeight());
    }
    
    void SpaceInvadersGame::OnInputPressed(const UI::InputType type, const wchar_t key)
    {
        std::wcout << "Key down\n";
    }

    void SpaceInvadersGame::CalculateFrameStats()
    {
        // Code computes the average frames per second, and also the 
        // average time it takes to render one frame.  These stats 
        // are appended to the window caption bar.

        static int frameCnt = 0;
        static float timeElapsed = 0.0f;

        frameCnt++;

        // Compute averages over one second period.
        if ((m_timer.TotalTime() - timeElapsed) >= 1.0f)
        {
            const float fps = (float)frameCnt; // fps = frameCnt / 1
            const float mspf = 1000.0f / fps;
            m_mainWindow.SetCaption(std::format(L"Space Invaders ({} fps, {} mspf)", fps, mspf));

            // Reset for next average.
            frameCnt = 0;
            timeElapsed += 1.0f;
        }
    }
}