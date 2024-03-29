module;

#include <stdexcept>
#include <iostream>
#include <Windows.h>

module core.ui.mainwindow;
import core.error.win32error;

namespace Core::UI
{
    MainWindow::MainWindow()
        : m_hwnd(nullptr),
        m_windowStyle(WS_OVERLAPPEDWINDOW)
    {}

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
        AdjustWindowRect(&r, m_windowStyle, false);
        // https://stackoverflow.com/questions/25879021/win32-client-size-and-an-incorrect-size
        m_hwnd = CreateWindowExW(
            0,
            L"D2DDemoApp",
            L"Direct2D Demo App",
            m_windowStyle,
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

    UINT64 MainWindow::RunMessageLoop()
    {
        MSG msg;
        while (GetMessageW(&msg, nullptr, 0, 0))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        return msg.wParam;
    }

    void MainWindow::ResizeWindow(const UINT width, const UINT height)
    {
        if (!m_hwnd)
            throw std::runtime_error(__FUNCSIG__": m_hwnd is null");
        const bool succeeded = SetWindowPos(
            m_hwnd, 
            HWND_TOP, 
            0, 
            0, 
            width, 
            height, 
            SWP_NOMOVE
        );
        if (!succeeded)
            throw Error::Win32Error(__FUNCSIG__": SetWindowPost() failed", GetLastError());
    }

    void MainWindow::ResizeClient(const UINT width, const UINT height)
    {
        if (!m_hwnd)
            throw std::runtime_error(__FUNCSIG__": m_hwnd is null");

        RECT r { 
            .right = static_cast<LONG>(width), 
            .bottom = static_cast<LONG>(height)
        };
        AdjustWindowRect(&r, m_windowStyle, false);
        // https://docs.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-setwindowpos
        const bool succeeded = SetWindowPos(
            m_hwnd, 
            HWND_TOP, 
            0, 
            0, 
            r.right - r.left, 
            r.bottom - r.top, 
            SWP_NOMOVE
        );
        if (!succeeded)
            throw Error::Win32Error(__FUNCSIG__": SetWindowPost() failed", GetLastError());
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
        // No need to pass this message on
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
