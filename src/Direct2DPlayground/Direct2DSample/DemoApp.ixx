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

export module DemoApp;

// See https://stackoverflow.com/questions/6126980/get-pointer-to-image-dos-header-with-getmodulehandle
// https://reverseengineering.stackexchange.com/questions/19660/is-there-any-way-to-get-my-own-image-base-without-calling-any-winapi-functions
//#ifndef HINST_THISCOMPONENT
//EXTERN_C IMAGE_DOS_HEADER __ImageBase;
//#define HINST_THISCOMPONENT ((HINSTANCE)&__ImageBase)
//#endif

export class DemoApp
{
    public:
        DemoApp();
        ~DemoApp();

        // Register the window class and call methods for instantiating drawing resources
        void Initialize();

        // Process and dispatch messages
        WPARAM RunMessageLoop();

    private:
        // Initialize device-independent resources.
        void CreateDeviceIndependentResources();

        // Initialize device-dependent resources.
        void CreateDeviceResources();

        // Release device-dependent resource.
        void DiscardDeviceResources();

        // Draw content.
        void OnRender();

        // Resize the render target.
        void OnResize(
            UINT width,
            UINT height
        );

        LRESULT HandleMessage(
            HWND hWnd,
            UINT message,
            WPARAM wParam,
            LPARAM lParam
        );

        // The windows procedure.
        static LRESULT CALLBACK WndProc(
            HWND hWnd,
            UINT message,
            WPARAM wParam,
            LPARAM lParam
        );

    private:
        HWND m_hwnd;
        Microsoft::WRL::ComPtr<ID2D1Factory> m_pDirect2dFactory;
        Microsoft::WRL::ComPtr<ID2D1HwndRenderTarget> m_pRenderTarget;
        Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> m_pLightSlateGrayBrush;
        Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> m_pCornflowerBlueBrush;

};