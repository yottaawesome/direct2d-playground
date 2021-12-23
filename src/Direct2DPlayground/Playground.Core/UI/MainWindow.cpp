module;

#include <stdexcept>
#include <iostream>
#include <Windows.h>

module core.ui.mainwindow;
import core.error.win32error;

namespace Core::UI
{
    MainWindow::MainWindow() : m_hwnd(nullptr) {}

    MainWindow::~MainWindow()
    {
        DestroyWindow(m_hwnd);
    }

    void MainWindow::Initialize()
    {
        // Initialize device-indpendent resources, such
        // as the Direct2D factory.
        CreateDeviceIndependentResources();

        // Register the window class.
        WNDCLASSEX wcex = { sizeof(WNDCLASSEX) };
        wcex.style = CS_HREDRAW | CS_VREDRAW;
        wcex.lpfnWndProc = MainWindow::WndProc;
        wcex.cbClsExtra = 0;
        wcex.cbWndExtra = sizeof(LONG_PTR);
        wcex.hInstance = GetModuleHandle(nullptr);// HINST_THISCOMPONENT;
        wcex.hbrBackground = nullptr;
        wcex.lpszMenuName = nullptr;
        wcex.hCursor = LoadCursor(nullptr, IDI_APPLICATION);
        wcex.lpszClassName = L"D2DDemoApp";
        RegisterClassExW(&wcex);

        // Because the CreateWindow function takes its size in pixels,
        // obtain the system DPI and use it to scale the window size.
        // The factory returns the current system DPI. This is also the value it will use
        // to create its own windows.
        /*
        * Deprecated
        * m_pDirect2dFactory->GetDesktopDpi(&dpiX, &dpiY);
        */
        const float dpi = static_cast<float>(GetDpiForWindow(GetDesktopWindow()));
        // Create the window.
        m_hwnd = CreateWindowExW(
            0,
            L"D2DDemoApp",
            L"Direct2D Demo App",
            WS_OVERLAPPEDWINDOW,
            CW_USEDEFAULT,
            CW_USEDEFAULT,
            static_cast<UINT>(ceil(640.f * dpi / 96.f)),
            static_cast<UINT>(ceil(480.f * dpi / 96.f)),
            nullptr,
            nullptr,
            GetModuleHandle(nullptr), //HINST_THISCOMPONENT,
            this
        );
        if (!m_hwnd)
            throw Core::Error::Win32Error(__FUNCSIG__": CreateWindowEx() failed", GetLastError());

        ShowWindow(m_hwnd, SW_SHOWNORMAL);
        UpdateWindow(m_hwnd);
    }

    WPARAM MainWindow::RunMessageLoop()
    {
        MSG msg;
        while (GetMessageW(&msg, nullptr, 0, 0))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        return msg.wParam;
    }

    LRESULT CALLBACK MainWindow::WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
    {
        // https://docs.microsoft.com/en-us/windows/win32/winmsg/wm-create
        if (message == WM_CREATE)
        {
            LPCREATESTRUCT pcs = reinterpret_cast<LPCREATESTRUCT>(lParam);
            MainWindow* pDemoApp = reinterpret_cast<MainWindow*>(pcs->lpCreateParams);
            SetWindowLongPtrW(
                hwnd,
                GWLP_USERDATA,
                reinterpret_cast<LONG_PTR>(pDemoApp)
            );
            return 0;
        }
        // Do not pass these messages to the Window object to be processed,
        // as it it's destroyed
        /*else if (message == WM_NCDESTROY)
        {
            return DefWindowProc(hwnd, message, wParam, lParam);
        }
        else if (message == WM_DESTROY)
        {
            return DefWindowProc(hwnd, message, wParam, lParam);
        }*/

        try
        {
            MainWindow* demoApp = reinterpret_cast<MainWindow*>(GetWindowLongPtrW(hwnd, GWLP_USERDATA));
            return demoApp
                ? demoApp->HandleMessage(hwnd, message, wParam, lParam)
                : DefWindowProcW(hwnd, message, wParam, lParam);
        }
        catch (const std::exception& ex)
        {
            std::wcerr << L"Exception in WndProc: " << ex.what() << std::endl;
            PostQuitMessage(1);
            return 0;
        }
    }
}
