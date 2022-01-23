module;

#include <functional>
#include <windows.h>
#include <d2d1.h>
#include <d2d1helper.h>
#include <dwrite.h>
#include <wincodec.h>
#include <wrl/client.h>

export module spaceinvaders.mainwindow;
import core.direct2d.d2d1factory;

export namespace SpaceInvaders
{
    class MainWindow
    {
        public:
            using OnResizeEvt = std::function<void()>;

        public:
            MainWindow();
            virtual ~MainWindow();

        public:
            virtual void Initialise();
            virtual HWND GetHandle() const noexcept;

        public:
            OnResizeEvt OnResizeEvent;

        protected:
            // Initialize device-independent resources.
            virtual void CreateDeviceIndependentResources();

            // Initialize device-dependent resources.
            virtual void CreateDeviceResources();

            // Release device-dependent resource.
            virtual void DiscardDeviceResources();

            // Draw content.
            virtual void OnRender();

            virtual void LoadTestBitmap();

            // Resize the render target.
            virtual void OnResize(const UINT width,const UINT height);

            virtual LRESULT HandleMessage(
                HWND hWnd,
                UINT message,
                WPARAM wParam,
                LPARAM lParam
            );
            
            virtual const WNDCLASSEX& GetClass() const noexcept;

            static LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

            virtual void Destroy();

        protected:
            /*Core::Direct2D::D2D1Factory m_pDirect2dFactory;
            Microsoft::WRL::ComPtr<ID2D1HwndRenderTarget> m_pRenderTarget;
            Microsoft::WRL::ComPtr<ID2D1Bitmap> m_bitmap;*/
            HWND m_hwnd;
            DWORD m_windowStyle;
    };
}
