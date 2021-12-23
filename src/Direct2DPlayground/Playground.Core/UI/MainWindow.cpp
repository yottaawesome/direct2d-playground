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
        if (m_hwnd)
            DestroyWindow(m_hwnd);
    }

    const WNDCLASSEX& MainWindow::GetClass() const noexcept
    {
        static const WNDCLASSEX wcex = {
            .cbSize = sizeof(WNDCLASSEX),
            .style = CS_HREDRAW | CS_VREDRAW,
            .lpfnWndProc = MainWindow::WndProc,
            .cbClsExtra = 0,
            .cbWndExtra = sizeof(LONG_PTR),
            .hInstance = GetModuleHandle(nullptr),// HINST_THISCOMPONENT;
            .hCursor = LoadCursor(nullptr, IDI_APPLICATION),
            .hbrBackground = nullptr,
            .lpszMenuName = nullptr,
            .lpszClassName = L"D2DDemoApp"
        };
        return wcex;
    }

    void MainWindow::Initialize()
    {
        // Initialize device-indpendent resources, such
        // as the Direct2D factory.
        CreateDeviceIndependentResources();

        // Register the window class, but allow subclasses the option
        // to override the WndProc
        WNDCLASSEX windowClass = GetClass();
        if (!windowClass.lpfnWndProc)
            windowClass.lpfnWndProc = MainWindow::WndProc;
        RegisterClassExW(&windowClass);

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
        RECT r{ 0 };
        r.right = 512;
        r.bottom = 512;
        AdjustWindowRect(&r, WS_OVERLAPPEDWINDOW, false);
        // https://stackoverflow.com/questions/25879021/win32-client-size-and-an-incorrect-size
        m_hwnd = CreateWindowExW(
            0,
            L"D2DDemoApp",
            L"Direct2D Demo App",
            WS_OVERLAPPEDWINDOW,
            CW_USEDEFAULT,
            CW_USEDEFAULT,
            r.right - r.left,
            //static_cast<UINT>(ceil(r.right * dpi / 96.f)),
            r.bottom - r.top,
            //static_cast<UINT>(ceil(r.bottom * dpi / 96.f)),
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
        else if (message == WM_DESTROY)
        {
            PostQuitMessage(0);
            return 0;
        }

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
