module;
// Windows Header Files:
#include <windows.h>

// C RunTime Header Files:
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <wchar.h>
#include <math.h>

#include <d2d1.h>
#include <d2d1helper.h>
#include <dwrite.h>
#include <wincodec.h>
#include <wrl/client.h>

export module MainWindow;
import core.ui.mainwindow2;
import core.direct2d.d2d1factory;

// See https://stackoverflow.com/questions/6126980/get-pointer-to-image-dos-header-with-getmodulehandle
// https://reverseengineering.stackexchange.com/questions/19660/is-there-any-way-to-get-my-own-image-base-without-calling-any-winapi-functions
//#ifndef HINST_THISCOMPONENT
//EXTERN_C IMAGE_DOS_HEADER __ImageBase;
//#define HINST_THISCOMPONENT ((HINSTANCE)&__ImageBase)
//#endif

export class MainWindow : public Core::UI::MainWindow2
{
    public:
        MainWindow();
        virtual ~MainWindow();

    protected:
        // Initialize device-independent resources.
        virtual void CreateDeviceIndependentResources() override;

        // Initialize device-dependent resources.
        virtual void CreateDeviceResources() override;

        // Release device-dependent resource.
        virtual void DiscardDeviceResources() override;

        // Draw content.
        virtual void OnRender() override;

        virtual void LoadTestBitmap();

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
        Microsoft::WRL::ComPtr<ID2D1Bitmap> m_bitmap;
};