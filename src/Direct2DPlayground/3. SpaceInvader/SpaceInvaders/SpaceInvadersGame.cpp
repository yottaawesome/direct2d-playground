module;

#include <vector>
#include <memory>
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
        : m_colourIndex(0), 
        m_timeElapsed(0),
        m_currentScene(0)
    {
        m_scenes.emplace_back(new SpaceInvaders::Scenes::MainMenu());
        m_scenes.emplace_back(new SpaceInvaders::Scenes::Level1());
        m_currentScene = m_scenes[0];
    }

    void SpaceInvadersGame::Initialise()
    {
        m_system.Window.OnResize = std::bind(
            &SpaceInvadersGame::OnResize, 
            this, 
            std::placeholders::_1, 
            std::placeholders::_2
        );
        m_system.Window.OnInputPressed = std::bind(
            &SpaceInvadersGame::OnInputPressed, 
            this, 
            std::placeholders::_1, 
            std::placeholders::_2
        );
        m_system.Window.Initialise();
        m_system.Factory.Initialise();
        m_system.Renderer = m_system.Factory.BindRenderTarget(
            m_system.Window.GetHandle(),
            m_system.Window.GetClientWidth(),
            m_system.Window.GetClientHeight()
        );
        m_currentScene->Initialise(m_system.Renderer);
    }

    UINT64 SpaceInvadersGame::RunMessageLoop()
    {
        m_system.Timer.Reset();

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
                m_system.Timer.Tick();
                m_currentScene->Update(m_system.Timer.DeltaTime());
                m_currentScene->Draw(m_system.Renderer);
                //RenderScene();
            }
        }

        return msg.wParam;
    }

    void SpaceInvadersGame::OnResize(const unsigned width, const unsigned height)
    {
        m_system.Renderer.Resize(
            m_system.Window.GetClientWidth(), 
            m_system.Window.GetClientHeight()
        );
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
        if ((m_system.Timer.TotalTime() - timeElapsed) < 1.0f)
            return;

        const float fps = (float)frameCnt; // fps = frameCnt / 1
        const float mspf = 1000.0f / fps;
        m_system.Window.SetCaption(std::format(L"Space Invaders ({} fps, {} mspf)", fps, mspf));

        // Reset for next average.
        frameCnt = 0;
        timeElapsed += 1.0f;
    }
}