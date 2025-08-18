export module core:ui_mainwindow2;
import :win32;
import :ui_mainwindow;

export namespace UI
{
    // This is  basically the same as MainWindow, but uses a different message loop
    // identical to the one in Intro To 3D Game Programming Using DX12's samples.
    struct MainWindow2 : MainWindow
    {
        virtual ~MainWindow2() {}

        // Process and dispatch messages
        virtual Win32::UINT64 RunMessageLoop() override
        {
            Win32::MSG msg = { 0 };
            while (msg.message != Win32::Messages::Quit)
            {
                // If there are Window messages then process them.
                if (Win32::PeekMessageW(&msg, 0, 0, 0, Win32::PmRemove))
                {
                    Win32::TranslateMessage(&msg);
                    Win32::DispatchMessageW(&msg);
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