module;

#include <Windows.h>

module core.ui.mainwindow2;

namespace Core::UI
{
    MainWindow2::~MainWindow2() {}

    UINT64 MainWindow2::RunMessageLoop()
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
}