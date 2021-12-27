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
            if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
            {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
            // Otherwise, do animation/game stuff.
            else
            {
                OnRender();
            }
        }

        return (int)msg.wParam;
    }
}