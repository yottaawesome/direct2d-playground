module;

// Windows Header Files:
#include <windows.h>

// C RunTime Header Files:
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <wchar.h>
#include <math.h>
#include <iostream>

#include <d2d1.h>
#include <d2d1helper.h>
#include <dwrite.h>
#include <wincodec.h>
#include <wrl/client.h>

module DemoApp;

DemoApp::DemoApp() :
    m_hwnd(nullptr),
    m_pDirect2dFactory(nullptr),
    m_pRenderTarget(nullptr),
    m_pLightSlateGrayBrush(nullptr),
    m_pCornflowerBlueBrush(nullptr)
{ }

DemoApp::~DemoApp()
{
    m_pDirect2dFactory = nullptr;
    m_pRenderTarget = nullptr;
    m_pLightSlateGrayBrush = nullptr;
    m_pCornflowerBlueBrush = nullptr;
}

void DemoApp::RunMessageLoop()
{
    MSG msg;

    while (GetMessageW(&msg, nullptr, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
}

HRESULT DemoApp::Initialize()
{
    // Initialize device-indpendent resources, such
    // as the Direct2D factory.
    HRESULT hr = CreateDeviceIndependentResources();

    if (SUCCEEDED(hr))
    {
        // Register the window class.
        WNDCLASSEX wcex = { sizeof(WNDCLASSEX) };
        wcex.style = CS_HREDRAW | CS_VREDRAW;
        wcex.lpfnWndProc = DemoApp::WndProc;
        wcex.cbClsExtra = 0;
        wcex.cbWndExtra = sizeof(LONG_PTR);
        wcex.hInstance = GetModuleHandle(nullptr);// HINST_THISCOMPONENT;
        wcex.hbrBackground = NULL;
        wcex.lpszMenuName = NULL;
        wcex.hCursor = LoadCursor(NULL, IDI_APPLICATION);
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
        hr = m_hwnd ? S_OK : E_FAIL;
        if (SUCCEEDED(hr))
        {
            ShowWindow(m_hwnd, SW_SHOWNORMAL);
            UpdateWindow(m_hwnd);
        }
    }

    return hr;
}

HRESULT DemoApp::CreateDeviceIndependentResources()
{
    // Create a Direct2D factory.
    HRESULT hr = D2D1CreateFactory(
        D2D1_FACTORY_TYPE_SINGLE_THREADED, 
        m_pDirect2dFactory.ReleaseAndGetAddressOf()
    );

    return hr;
}

HRESULT DemoApp::CreateDeviceResources()
{
    if (m_pRenderTarget)
        return S_OK;

    RECT rc;
    GetClientRect(m_hwnd, &rc);

    D2D1_SIZE_U size = D2D1::SizeU(
        rc.right - rc.left,
        rc.bottom - rc.top
    );

    // Create a Direct2D render target.
    HRESULT hr = m_pDirect2dFactory->CreateHwndRenderTarget(
        D2D1::RenderTargetProperties(),
        D2D1::HwndRenderTargetProperties(m_hwnd, size),
        &m_pRenderTarget
    );
    if (SUCCEEDED(hr))
    {
        // Create a gray brush.
        hr = m_pRenderTarget->CreateSolidColorBrush(
            D2D1::ColorF(D2D1::ColorF::LightSlateGray),
            &m_pLightSlateGrayBrush
        );
    }
    if (SUCCEEDED(hr))
    {
        // Create a blue brush.
        hr = m_pRenderTarget->CreateSolidColorBrush(
            D2D1::ColorF(D2D1::ColorF::CornflowerBlue),
            m_pCornflowerBlueBrush.ReleaseAndGetAddressOf()
        );
    }

    return hr;
}

void DemoApp::DiscardDeviceResources()
{
    m_pRenderTarget = nullptr;
    m_pLightSlateGrayBrush = nullptr;
    m_pCornflowerBlueBrush = nullptr;
}

LRESULT CALLBACK DemoApp::WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    // https://docs.microsoft.com/en-us/windows/win32/winmsg/wm-create
    if (message == WM_CREATE)
    {
        LPCREATESTRUCT pcs = (LPCREATESTRUCT)lParam;
        DemoApp* pDemoApp = (DemoApp*)pcs->lpCreateParams;
        SetWindowLongW(
            hwnd,
            GWLP_USERDATA,
            reinterpret_cast<LONG_PTR>(pDemoApp)
        );

        return 0;
    }

    DemoApp* pDemoApp = reinterpret_cast<DemoApp*>(GetWindowLongW(hwnd, GWLP_USERDATA));
    if(!pDemoApp)
        return DefWindowProc(hwnd, message, wParam, lParam);

    switch (message)
    {
        // https://docs.microsoft.com/en-us/windows/win32/winmsg/wm-size
        case WM_SIZE:
        {
            UINT width = LOWORD(lParam);
            UINT height = HIWORD(lParam);
            pDemoApp->OnResize(width, height);
            return 0;
        }
         
        // https://docs.microsoft.com/en-us/windows/win32/gdi/wm-displaychange
        case WM_DISPLAYCHANGE:
        {
            InvalidateRect(hwnd, NULL, FALSE);
            return 0;
        }

        // https://docs.microsoft.com/en-us/windows/win32/gdi/wm-paint
        case WM_PAINT:
        {
            pDemoApp->OnRender();
            ValidateRect(hwnd, NULL);
            return 0;
        }

        // https://docs.microsoft.com/en-us/windows/win32/winmsg/wm-destroy
        case WM_DESTROY:
        {
            PostQuitMessage(0);
            return 0;
        }

        default:
            return DefWindowProc(hwnd, message, wParam, lParam);
    }
}

HRESULT DemoApp::OnRender()
{
    HRESULT hr = S_OK;

    hr = CreateDeviceResources();
    if (SUCCEEDED(hr))
    {
        m_pRenderTarget->BeginDraw();

        m_pRenderTarget->SetTransform(D2D1::Matrix3x2F::Identity());

        m_pRenderTarget->Clear(D2D1::ColorF(D2D1::ColorF::White));
        D2D1_SIZE_F rtSize = m_pRenderTarget->GetSize();
        // Draw a grid background.
        int width = static_cast<int>(rtSize.width);
        int height = static_cast<int>(rtSize.height);

        for (int x = 0; x < width; x += 10)
        {
            m_pRenderTarget->DrawLine(
                D2D1::Point2F(static_cast<FLOAT>(x), 0.0f),
                D2D1::Point2F(static_cast<FLOAT>(x), rtSize.height),
                m_pLightSlateGrayBrush.Get(),
                0.5f
            );
        }

        for (int y = 0; y < height; y += 10)
        {
            m_pRenderTarget->DrawLine(
                D2D1::Point2F(0.0f, static_cast<FLOAT>(y)),
                D2D1::Point2F(rtSize.width, static_cast<FLOAT>(y)),
                m_pLightSlateGrayBrush.Get(),
                0.5f
            );
        }
        // Draw two rectangles.
        D2D1_RECT_F rectangle1 = D2D1::RectF(
            rtSize.width / 2 - 50.0f,
            rtSize.height / 2 - 50.0f,
            rtSize.width / 2 + 50.0f,
            rtSize.height / 2 + 50.0f
        );

        D2D1_RECT_F rectangle2 = D2D1::RectF(
            rtSize.width / 2 - 100.0f,
            rtSize.height / 2 - 100.0f,
            rtSize.width / 2 + 100.0f,
            rtSize.height / 2 + 100.0f
        );

        // Draw a filled rectangle.
        m_pRenderTarget->FillRectangle(&rectangle1, m_pLightSlateGrayBrush.Get());

        // Draw the outline of a rectangle.
        m_pRenderTarget->DrawRectangle(&rectangle2, m_pCornflowerBlueBrush.Get());

        hr = m_pRenderTarget->EndDraw();
    }

    if (hr == D2DERR_RECREATE_TARGET)
    {
        std::wcout << L"Recreating target\n";
        hr = S_OK;
        DiscardDeviceResources();
    }

    return hr;
}

void DemoApp::OnResize(UINT width, UINT height)
{
    if (m_pRenderTarget)
    {
        // Note: This method can fail, but it's okay to ignore the
        // error here, because the error will be returned again
        // the next time EndDraw is called.
        m_pRenderTarget->Resize(D2D1::SizeU(width, height));
    }
}