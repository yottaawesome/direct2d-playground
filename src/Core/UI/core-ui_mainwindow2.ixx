module;

#include <Windows.h>

export module core:ui_mainwindow2;
import :ui_mainwindow;

export namespace UI
{
    // This is  basically the same as MainWindow, but uses a different message loop
    // identical to the one in Intro To 3D Game Programming Using DX12's samples.
    struct MainWindow2 : MainWindow
    {
        virtual ~MainWindow2() {}

        // Process and dispatch messages
        virtual UINT64 RunMessageLoop() override
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
                    OnRender();
                }
            }

            return msg.wParam;
        }
    };
}