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
import core.error;
import core.wic.wicimagingfactory;

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

WPARAM DemoApp::RunMessageLoop()
{
    MSG msg;
    while (GetMessageW(&msg, nullptr, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return msg.wParam;
}

void DemoApp::Initialize()
{
    // Initialize device-indpendent resources, such
    // as the Direct2D factory.
    CreateDeviceIndependentResources();

    // Register the window class.
    WNDCLASSEX wcex = { sizeof(WNDCLASSEX) };
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = DemoApp::WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = sizeof(LONG_PTR);
    wcex.hInstance = GetModuleHandle(nullptr);// HINST_THISCOMPONENT;
    wcex.hbrBackground = nullptr;
    wcex.lpszMenuName = nullptr;
    wcex.hCursor = LoadCursor(nullptr, IDI_APPLICATION);
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
    if (!m_hwnd)
        throw Core::Error::Win32Error(__FUNCSIG__": CreateWindowEx() failed", GetLastError());
    
    ShowWindow(m_hwnd, SW_SHOWNORMAL);
    UpdateWindow(m_hwnd);
}

void DemoApp::CreateDeviceIndependentResources()
{
    // Create a Direct2D factory.
    const HRESULT hr = D2D1CreateFactory(
        D2D1_FACTORY_TYPE_SINGLE_THREADED, 
        m_pDirect2dFactory.ReleaseAndGetAddressOf()
    );
    if (FAILED(hr)) 
        throw Core::Error::COMError(__FUNCSIG__": D2D1CreateFactory() failed", hr);
}

void DemoApp::CreateDeviceResources()
{
    if (m_pRenderTarget)
        return;

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
    if (FAILED(hr)) 
        throw Core::Error::COMError(__FUNCSIG__": CreateHwndRenderTarget() failed", hr);

    // Create a gray brush.
    hr = m_pRenderTarget->CreateSolidColorBrush(
        D2D1::ColorF(D2D1::ColorF::LightSlateGray),
        &m_pLightSlateGrayBrush
    );
    if (FAILED(hr)) 
        throw Core::Error::COMError(__FUNCSIG__": CreateSolidColorBrush() failed [1]", hr);

    // Create a blue brush.
    hr = m_pRenderTarget->CreateSolidColorBrush(
        D2D1::ColorF(D2D1::ColorF::CornflowerBlue),
        m_pCornflowerBlueBrush.ReleaseAndGetAddressOf()
    );
    if (FAILED(hr)) 
        throw Core::Error::COMError(__FUNCSIG__": CreateSolidColorBrush() failed [2]", hr);

    //LoadTestBitmap();
}

void DemoApp::DiscardDeviceResources()
{
    m_pRenderTarget = nullptr;
    m_pLightSlateGrayBrush = nullptr;
    m_pCornflowerBlueBrush = nullptr;
}

LRESULT DemoApp::HandleMessage(
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
            const UINT width = LOWORD(lParam);
            const UINT height = HIWORD(lParam);
            OnResize(width, height);
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
            OnRender();
            ValidateRect(hwnd, nullptr);
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

LRESULT CALLBACK DemoApp::WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    // https://docs.microsoft.com/en-us/windows/win32/winmsg/wm-create
    if (message == WM_CREATE)
    {
        LPCREATESTRUCT pcs = reinterpret_cast<LPCREATESTRUCT>(lParam);
        DemoApp* pDemoApp = reinterpret_cast<DemoApp*>(pcs->lpCreateParams);
        SetWindowLongPtrW(
            hwnd,
            GWLP_USERDATA,
            reinterpret_cast<LONG_PTR>(pDemoApp)
        );
        return 0;
    }

    try
    {
        DemoApp* demoApp = reinterpret_cast<DemoApp*>(GetWindowLongPtrW(hwnd, GWLP_USERDATA));
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

void DemoApp::LoadTestBitmap()
{
    // https://docs.microsoft.com/en-us/windows/win32/Direct2D/how-to-load-a-direct2d-bitmap-from-a-file
    Core::WIC::WICImagingFactory factory;
    Microsoft::WRL::ComPtr<IWICBitmapDecoder> pDecoder =
        factory.CreateDecoderFromFilename(LR"(C:\Users\Royal\Desktop\lena.bmp)");

    Microsoft::WRL::ComPtr<IWICBitmapFrameDecode> pSource;
    // Create the initial frame.
    HRESULT hr = pDecoder->GetFrame(0, &pSource);
    if (FAILED(hr))
        throw Core::Error::COMError("GetFrame() failed", hr);

    Microsoft::WRL::ComPtr<IWICFormatConverter> pConverter = factory.CreateFormatConverter();

    hr = pConverter->Initialize(
        pSource.Get(),
        GUID_WICPixelFormat32bppPBGRA,
        WICBitmapDitherTypeNone,
        nullptr,
        0.f,
        WICBitmapPaletteTypeMedianCut
    );
    if (FAILED(hr))
        throw Core::Error::COMError("Initialize() failed", hr);

    hr = m_pRenderTarget->CreateBitmapFromWicBitmap(
        pConverter.Get(),
        nullptr,
        &m_bitmap
    );
    if (FAILED(hr))
        throw Core::Error::COMError("CreateBitmapFromWicBitmap() failed", hr);
}

void DemoApp::OnRender()
{
    CreateDeviceResources();
 
    m_pRenderTarget->BeginDraw();
    m_pRenderTarget->SetTransform(D2D1::Matrix3x2F::Identity());
    m_pRenderTarget->Clear(D2D1::ColorF(D2D1::ColorF::White));

    // Draw a grid background.
    D2D1_SIZE_F rtSize = m_pRenderTarget->GetSize();
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

    // This is our bitmap test
    //m_pRenderTarget->DrawBitmap(m_bitmap.Get());
    
    if (const HRESULT hr = m_pRenderTarget->EndDraw(); hr == D2DERR_RECREATE_TARGET)
    {
        std::wcout << L"Recreating target\n";
        DiscardDeviceResources();
    }
    else if (FAILED(hr))
    {
        throw Core::Error::COMError(__FUNCSIG__": EndDraw() failed", hr);
    }
}

void DemoApp::OnResize(const UINT width, const UINT height)
{
    if (!m_pRenderTarget)
        return;
    // Note: This method can fail, but it's okay to ignore the
    // error here, because the error will be returned again
    // the next time EndDraw is called.
    m_pRenderTarget->Resize(D2D1::SizeU(width, height));
}