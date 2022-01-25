module;

#include <iostream>
#include <string>
#include <windows.h>
#include <d2d1.h>
#include <d2d1helper.h>
#include <dwrite.h>
#include <wincodec.h>
#include <wrl/client.h>

module spaceinvaders.ui.mainwindow;
import core.error;
import core.wic.wicimagingfactory;

namespace SpaceInvaders::UI
{
    MainWindow::MainWindow()
        : m_hwnd(nullptr),
        m_windowStyle(WS_OVERLAPPEDWINDOW),
        OnResize([](auto, auto){}),
        OnInputPressed([](auto, auto){}),
        OnInputReleased([](auto, auto){})
    {
    }

    MainWindow::~MainWindow()
    {
        Destroy();
    }

    void MainWindow::Destroy()
    {
        if (m_hwnd)
            DestroyWindow(m_hwnd);
    }

    LRESULT MainWindow::HandleMessage(
        HWND hwnd,
        UINT message,
        WPARAM wParam,
        LPARAM lParam
    )
    {
        switch (message)
        {
            // https://docs.microsoft.com/en-us/windows/win32/winmsg/wm-size
            case WM_SIZE:
            {
                OnResize(LOWORD(lParam), HIWORD(lParam));
                return 0;
            }

            // https://docs.microsoft.com/en-us/windows/win32/gdi/wm-displaychange
            case WM_DISPLAYCHANGE:
            {
                InvalidateRect(hwnd, nullptr, false);
                return 0;
            }

            // https://docs.microsoft.com/en-us/windows/win32/gdi/wm-paint
            case WM_PAINT:
            {
                //OnRender();
                ValidateRect(hwnd, nullptr);
                return 0;
            }

            // https://docs.microsoft.com/en-us/windows/win32/winmsg/wm-destroy
            case WM_DESTROY:
            {
                PostQuitMessage(0);
                return 0;
            }

            // https://docs.microsoft.com/en-us/windows/win32/inputdev/about-keyboard-input#keystroke-message-flags
            // https://docs.microsoft.com/en-us/windows/win32/inputdev/wm-char
            // Sent first
            case WM_KEYDOWN:
            {
                OnInputPressed(InputType::KeyDown, (wchar_t)wParam);
                return 0;
            }

            // Sent after WM_KEYDOWN
            case WM_CHAR:
            {
                //OnInput(InputType::Key, (wchar_t)wParam);
                return 0;
            }

            // Sent after key is released
            case WM_KEYUP:
            {
                OnInputReleased(InputType::KeyUp, (wchar_t)wParam);
                return 0;
            }

            // There's also the following seer the above links
            // WM_SYSKEYDOWN
            // WM_SYSCHAR
            // WM_SYSKEYUP

            default:
                return DefWindowProc(hwnd, message, wParam, lParam);
        }
    }

    void MainWindow::OnRender()
    {
        //CreateDeviceResources();

        //m_pRenderTarget->BeginDraw();
        //// https://docs.microsoft.com/en-us/windows/win32/Direct2D/how-to-rotate
        //m_pRenderTarget->SetTransform(D2D1::Matrix3x2F::Identity());
        //m_pRenderTarget->Clear(D2D1::ColorF(D2D1::ColorF::Red));

        //m_pRenderTarget->DrawBitmap(m_bitmap.Get());
        //auto x = D2D1::Matrix3x2F::Identity();
        //x = x.Translation(D2D1_SIZE_F(22, 22));
        //m_pRenderTarget->SetTransform(x);
        //m_pRenderTarget->DrawBitmap(m_bitmap.Get());

        //if (const HRESULT hr = m_pRenderTarget->EndDraw(); hr == D2DERR_RECREATE_TARGET)
        //{
        //    std::wcout << L"Recreating target\n";
        //    DiscardDeviceResources();
        //}
        //else if (FAILED(hr))
        //{
        //    throw Core::Error::COMError(__FUNCSIG__": EndDraw() failed", hr);
        //}
    }

    void MainWindow::Initialise()
    {
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

    HWND MainWindow::GetHandle() const noexcept
    {
        return m_hwnd;
    }

    unsigned MainWindow::GetWidth() const
    {
        if (!m_hwnd)
            return 0;
        RECT r;
        if (!GetWindowRect(m_hwnd, &r))
            throw Core::Error::Win32Error(__FUNCSIG__": GetClientRect() failed", GetLastError());
        return r.right - r.left;
    }

    unsigned MainWindow::GetHeight() const
    {
        if (!m_hwnd)
            return 0;
        RECT r;
        if (!GetWindowRect(m_hwnd, &r))
            throw Core::Error::Win32Error(__FUNCSIG__": GetClientRect() failed", GetLastError());
        return r.bottom - r.top;
    }

    unsigned MainWindow::GetClientWidth() const
    {
        if (!m_hwnd)
            return 0;
        RECT r;
        if (!GetClientRect(m_hwnd, &r))
            throw Core::Error::Win32Error(__FUNCSIG__": GetClientRect() failed", GetLastError());
        return r.right - r.left;
    }

    unsigned MainWindow::GetClientHeight() const
    {
        if (!m_hwnd)
            return 0;
        RECT r;
        if (!GetClientRect(m_hwnd, &r))
            throw Core::Error::Win32Error(__FUNCSIG__": GetClientRect() failed", GetLastError());
        return r.bottom - r.top;
    }

    void MainWindow::SetCaption(const std::wstring& caption)
    {
        SetWindowText(m_hwnd, caption.c_str());
    }
}
