module;

#include <Windows.h>

export module core.ui.mainwindow2;
import core.ui.mainwindow;

export namespace Core::UI
{
    // This is  basically the same as MainWindow, but uses a different message loop
    // identical to the one in Intro To 3D Game Programming Using DX12's samples.
    class MainWindow2 : public MainWindow
    {
        public:
            virtual ~MainWindow2();

            // Process and dispatch messages
            virtual UINT64 RunMessageLoop() override;
    };
}