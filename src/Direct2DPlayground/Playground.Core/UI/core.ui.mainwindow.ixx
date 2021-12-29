module;

#include <Windows.h>

export module core.ui.mainwindow;

export namespace Core::UI
{
    class MainWindow
    {
        public:
            MainWindow();
            virtual ~MainWindow();

            // Register the window class and call methods for instantiating drawing resources
            virtual void Initialize();

            // Process and dispatch messages
            virtual UINT64 RunMessageLoop();

            virtual void ResizeWindow(const UINT width, const UINT height);
            virtual void ResizeClient(const UINT width, const UINT height);

        protected:
            virtual const WNDCLASSEX& GetClass() const noexcept;

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
                const UINT width,
                const UINT height
            ) = 0;

            virtual LRESULT HandleMessage(
                HWND hWnd,
                UINT message,
                WPARAM wParam,
                LPARAM lParam
            ) = 0;

        protected:
            // The windows procedure.
            static LRESULT CALLBACK WndProc(
                HWND hWnd,
                UINT message,
                WPARAM wParam,
                LPARAM lParam
            );

        protected:
            HWND m_hwnd;
    };
}