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

DemoApp::DemoApp() : Core::UI::MainWindow() { }

DemoApp::~DemoApp()
{
    m_pDirect2dFactory = nullptr;
    m_pRenderTarget = nullptr;
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

    LoadTestBitmap();
}

void DemoApp::DiscardDeviceResources()
{
    m_pRenderTarget = nullptr;
    m_bitmap = nullptr;
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
            //static bool firstRender = true;
            //if (firstRender)
            //{
            //    firstRender = false;
            //    D2D1_SIZE_U size = m_bitmap->GetPixelSize();
            //    RECT r{ 0 };
            //    r.right = size.width;
            //    r.bottom = size.height;
            //    AdjustWindowRect(
            //        &r, WS_CAPTION, false
            //    );
            ////    //PostMessageW(m_hwnd, WM_SIZE, 0, MAKELPARAM(1000, r.bottom));
            //    SetWindowPos(m_hwnd, HWND_TOP, 0, 0, r.right, r.bottom, SWP_NOMOVE);
            //    m_pRenderTarget->Resize(D2D1::SizeU(r.right, r.bottom));
            //}

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

    // This is our bitmap test
    //RECT rc;
    //GetClientRect(m_hwnd, &rc);
    m_pRenderTarget->DrawBitmap(m_bitmap.Get());
    
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