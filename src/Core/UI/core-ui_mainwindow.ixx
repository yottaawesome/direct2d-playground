export module core:ui_mainwindow;
import std;
import :win32;
import :error_win32error;

export namespace UI
{
    class MainWindow
    {
    public:
        virtual ~MainWindow()
        {
            if (m_hwnd)
                Win32::DestroyWindow(m_hwnd);
        }

        MainWindow()
            : m_hwnd(nullptr),
            m_windowStyle(Win32::WsOverlappedWindow)
        { }

        // Register the window class and call methods for instantiating drawing resources
        virtual void Initialize()
        {
            // Initialize device-indpendent resources, such
            // as the Direct2D factory.
            CreateDeviceIndependentResources();

            // Register the window class, but allow subclasses the option
            // to override the WndProc
            Win32::WNDCLASSEX windowClass = GetClass();
            if (!windowClass.lpfnWndProc)
                windowClass.lpfnWndProc = MainWindow::WndProc;
            Win32::RegisterClassExW(&windowClass);

            // Because the CreateWindow function takes its size in pixels,
            // obtain the system DPI and use it to scale the window size.
            // The factory returns the current system DPI. This is also the value it will use
            // to create its own windows.
            /*
            * Deprecated
            * m_pDirect2dFactory->GetDesktopDpi(&dpiX, &dpiY);
            */
            float dpi = static_cast<float>(Win32::GetDpiForWindow(Win32::GetDesktopWindow()));
            // Create the window.
            Win32::RECT r{ 0 };
            r.right = 512;
            r.bottom = 512;
            Win32::AdjustWindowRect(&r, m_windowStyle, false);
            // https://stackoverflow.com/questions/25879021/win32-client-size-and-an-incorrect-size
            m_hwnd = Win32::CreateWindowExW(
                0,
                L"D2DDemoApp",
                L"Direct2D Demo App",
                m_windowStyle,
                Win32::CwUseDefault,
                Win32::CwUseDefault,
                r.right - r.left,
                //static_cast<UINT>(ceil(r.right * dpi / 96.f)),
                r.bottom - r.top,
                //static_cast<UINT>(ceil(r.bottom * dpi / 96.f)),
                nullptr,
                nullptr,
                Win32::GetModuleHandleW(nullptr), //HINST_THISCOMPONENT,
                this
            );
            if (!m_hwnd)
                throw Error::Win32Error("CreateWindowEx() failed", Win32::GetLastError());

            Win32::ShowWindow(m_hwnd, Win32::SwShowNormal);
            Win32::UpdateWindow(m_hwnd);
        }

        // Process and dispatch messages
        virtual Win32::UINT64 RunMessageLoop()
        {
            Win32::MSG msg;
            while (Win32::GetMessageW(&msg, nullptr, 0, 0))
            {
                Win32::TranslateMessage(&msg);
                Win32::DispatchMessageW(&msg);
            }
            return msg.wParam;
        }

        virtual void ResizeWindow(const UINT width, const UINT height)
        {
            if (!m_hwnd)
                throw std::runtime_error("m_hwnd is null");
            bool succeeded = Win32::SetWindowPos(
                m_hwnd,
                Win32::HwndTop,
                0,
                0,
                width,
                height,
                Win32::SwpNoMove
            );
            if (!succeeded)
                throw Error::Win32Error("SetWindowPost() failed", GetLastError());
        }

        virtual void ResizeClient(const UINT width, const UINT height)
        {
            if (!m_hwnd)
                throw std::runtime_error("m_hwnd is null");

            Win32::RECT r{
                .right = static_cast<LONG>(width),
                .bottom = static_cast<LONG>(height)
            };
            AdjustWindowRect(&r, m_windowStyle, false);
            // https://docs.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-setwindowpos
            bool succeeded = Win32::SetWindowPos(
                m_hwnd,
                Win32::HwndTop,
                0,
                0,
                r.right - r.left,
                r.bottom - r.top,
                Win32::SwpNoMove
            );
            if (!succeeded)
                throw Error::Win32Error("SetWindowPost() failed", Win32::GetLastError());
        }

    protected:
        virtual const Win32::WNDCLASSEX& GetClass() const noexcept
        {
            static const Win32::WNDCLASSEX wcex{
                .cbSize = sizeof(Win32::WNDCLASSEX),
                .style = Win32::CsHRedraw | Win32::CsVRedraw,
                .lpfnWndProc = MainWindow::WndProc,
                .cbClsExtra = 0,
                .cbWndExtra = sizeof(Win32::LONG_PTR),
                .hInstance = Win32::GetModuleHandleW(nullptr),// HINST_THISCOMPONENT;
                .hCursor = Win32::LoadCursorW(nullptr, Win32::IdiApplication),
                .hbrBackground = nullptr,
                .lpszMenuName = nullptr,
                .lpszClassName = L"D2DDemoApp"
            };
            return wcex;
        }

        // Initialize device-independent resources.
        virtual void CreateDeviceIndependentResources() = 0;

        // Initialize device-dependent resources.
        virtual void CreateDeviceResources() = 0;

        // Release device-dependent resource.
        virtual void DiscardDeviceResources() = 0;

        // Draw content.
        virtual void OnRender() = 0;

        // Resize the render target.
        virtual void OnResize(
            const Win32::UINT width,
            const Win32::UINT height
        ) = 0;

        virtual Win32::LRESULT HandleMessage(
            Win32::HWND hWnd,
            Win32::UINT message,
            Win32::WPARAM wParam,
            Win32::LPARAM lParam
        ) = 0;

        // The windows procedure.
        static LRESULT WndProc(
            Win32::HWND hwnd,
            Win32::UINT message,
            Win32::WPARAM wParam,
            Win32::LPARAM lParam
        )
        {
            // https://docs.microsoft.com/en-us/windows/win32/winmsg/wm-create
            if (message == Win32::Messages::Create)
            {
                Win32::LPCREATESTRUCT pcs = reinterpret_cast<Win32::LPCREATESTRUCT>(lParam);
                MainWindow* pDemoApp = reinterpret_cast<MainWindow*>(pcs->lpCreateParams);
                Win32::SetWindowLongPtrW(
                    hwnd,
                    Win32::GwlpUserData,
                    reinterpret_cast<LONG_PTR>(pDemoApp)
                );
                return 0;
            }
            // No need to pass this message on
            else if (message == Win32::Messages::Destroy)
            {
                Win32::PostQuitMessage(0);
                return 0;
            }

            try
            {
                MainWindow* demoApp = reinterpret_cast<MainWindow*>(Win32::GetWindowLongPtrW(hwnd, Win32::GwlpUserData));
                return demoApp
                    ? demoApp->HandleMessage(hwnd, message, wParam, lParam)
                    : Win32::DefWindowProcW(hwnd, message, wParam, lParam);
            }
            catch (const std::exception& ex)
            {
                std::wcerr << L"Exception in WndProc: " << ex.what() << std::endl;
                Win32::PostQuitMessage(1);
                return 0;
            }
        }

        Win32::HWND m_hwnd;
        Win32::DWORD m_windowStyle;
    };
}