module;

#include <iostream>
#include <windows.h>

#include <d2d1.h>
#include <d2d1helper.h>
#include <dwrite.h>
#include <wincodec.h>
#include <wrl/client.h>

module DemoApp;
import core.error;
import core.wic.wicimagingfactory;

DemoApp::DemoApp() 
    : Core::UI::MainWindow2(),  
    m_stringToRender(L"Hello, DirectWrite!")
{ }

DemoApp::~DemoApp()
{
    m_pDirect2dFactory.Close();
    m_pRenderTarget = nullptr;
}

void DemoApp::CreateDeviceIndependentResources()
{
    m_pDirect2dFactory.Initialise(D2D1_FACTORY_TYPE_SINGLE_THREADED);

    HRESULT hr = DWriteCreateFactory(
        DWRITE_FACTORY_TYPE_SHARED,
        __uuidof(IDWriteFactory),
        &pDWriteFactory
    );
    if (FAILED(hr))
        throw Core::Error::COMError("DWriteCreateFactory() failed", hr);

    hr = pDWriteFactory->CreateTextFormat(
        L"Gabriola",                // Font family name.
        NULL,                       // Font collection (NULL sets it to use the system font collection).
        DWRITE_FONT_WEIGHT_REGULAR,
        DWRITE_FONT_STYLE_NORMAL,
        DWRITE_FONT_STRETCH_NORMAL,
        72.0f,
        L"en-us",
        &pTextFormat
    );
    if (FAILED(hr))
        throw Core::Error::COMError("CreateTextFormat() failed", hr);

    // Center align (horizontally) the text.
    hr = pTextFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
    if (FAILED(hr))
        throw Core::Error::COMError("SetTextAlignment() failed", hr);

    hr = pTextFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
    if (FAILED(hr))
        throw Core::Error::COMError("SetParagraphAlignment() failed", hr);
}

void DemoApp::CreateDeviceResources()
{
    if (m_pRenderTarget)
        return;

    RECT rc;
    GetClientRect(m_hwnd, &rc);

    // Create a Direct2D render target.
    m_pRenderTarget = m_pDirect2dFactory.CreateHwndRenderTarget(
        m_hwnd, 
        rc.right - rc.left, 
        rc.bottom - rc.top
    );

    HRESULT hr = m_pRenderTarget->CreateSolidColorBrush(
        D2D1::ColorF(D2D1::ColorF::Black),
        &pBlackBrush
    );
    if (FAILED(hr))
        throw Core::Error::COMError("SetParagraphAlignment() failed", hr);
}

void DemoApp::DiscardDeviceResources()
{
    m_pRenderTarget = nullptr;
}

void DemoApp::DrawText()
{
    const float dpi = static_cast<float>(GetDpiForWindow(GetDesktopWindow()));
    RECT rc;
    GetClientRect(m_hwnd, &rc);
    D2D1_RECT_F layoutRect = D2D1::RectF(
        static_cast<float>(rc.left),
        static_cast<float>(rc.top),
        static_cast<float>(rc.right - rc.left),
        static_cast<float>(rc.bottom - rc.top)
    );
    m_pRenderTarget->DrawText(
        &m_stringToRender[0],        // The string to render.
        m_stringToRender.size(),    // The string's length.
        pTextFormat.Get(),    // The text format.
        layoutRect,       // The region of the window where the text will be rendered.
        pBlackBrush.Get()     // The brush used to draw the text.
    );
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

        default:
            return DefWindowProc(hwnd, message, wParam, lParam);
    }
}

void DemoApp::OnRender()
{
    CreateDeviceResources();
 
    m_pRenderTarget->BeginDraw();
    m_pRenderTarget->SetTransform(D2D1::Matrix3x2F::Identity());
    m_pRenderTarget->Clear(D2D1::ColorF(D2D1::ColorF::BlueViolet));
    
    DrawText();

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