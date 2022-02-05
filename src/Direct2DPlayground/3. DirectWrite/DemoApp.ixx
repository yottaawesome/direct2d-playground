module;

#include <string>
#include <windows.h>
#include <d2d1.h>
#include <dwrite.h>
#include <wrl/client.h>

export module DemoApp;
import core.ui.mainwindow2;
import core.direct2d.d2d1factory;

// This is based on https://docs.microsoft.com/en-us/windows/win32/directwrite/getting-started-with-directwrite
export class DemoApp : public Core::UI::MainWindow2
{
    public:
        DemoApp();
        virtual ~DemoApp();

    protected:
        // Initialize device-independent resources.
        virtual void CreateDeviceIndependentResources() override;

        // Initialize device-dependent resources.
        virtual void CreateDeviceResources() override;

        // Release device-dependent resource.
        virtual void DiscardDeviceResources() override;

        virtual void DrawText();

        // Draw content.
        virtual void OnRender() override;

        // Resize the render target.
        virtual void OnResize(
            const UINT width,
            const UINT height
        ) override;

        virtual LRESULT HandleMessage(
            HWND hWnd,
            UINT message,
            WPARAM wParam,
            LPARAM lParam
        ) override;

    protected:
        Core::Direct2D::D2D1Factory m_pDirect2dFactory;
        Microsoft::WRL::ComPtr<ID2D1HwndRenderTarget> m_pRenderTarget;
        Microsoft::WRL::ComPtr<IDWriteFactory> pDWriteFactory;
        Microsoft::WRL::ComPtr<IDWriteTextFormat> pTextFormat;
        Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> pBlackBrush;
        std::wstring m_stringToRender;
};