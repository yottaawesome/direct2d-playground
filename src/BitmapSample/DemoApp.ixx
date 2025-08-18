module;
// Windows Header Files:
#include <windows.h>
#include <d2d1.h>
#include <d2d1helper.h>
#include <dwrite.h>
#include <wincodec.h>
#include <wrl/client.h>

export module DemoApp;
import std;
import core;

// See https://stackoverflow.com/questions/6126980/get-pointer-to-image-dos-header-with-getmodulehandle
// https://reverseengineering.stackexchange.com/questions/19660/is-there-any-way-to-get-my-own-image-base-without-calling-any-winapi-functions
//#ifndef HINST_THISCOMPONENT
//EXTERN_C IMAGE_DOS_HEADER __ImageBase;
//#define HINST_THISCOMPONENT ((HINSTANCE)&__ImageBase)
//#endif

export class DemoApp : public Core::UI::MainWindow2
{
public:
    DemoApp() : Core::UI::MainWindow2() {}

    ~DemoApp()
    {
        m_pDirect2dFactory.Close();
        m_pRenderTarget = nullptr;
    }

protected:
    // Initialize device-independent resources.
    void CreateDeviceIndependentResources() override
    {
        m_pDirect2dFactory.Initialise(D2D1_FACTORY_TYPE_SINGLE_THREADED);
    }

    // Initialize device-dependent resources.
    void CreateDeviceResources() override
    {
        if (m_pRenderTarget)
            return;

        RECT rc;
        GetClientRect(m_hwnd, &rc);

        // Create a Direct2D render target.
        m_pRenderTarget = m_pDirect2dFactory.CreateHwndRenderTarget(m_hwnd, rc.right - rc.left, rc.bottom - rc.top);
        LoadTestBitmap();
    }

    // Release device-dependent resource.
    void DiscardDeviceResources() override
    {
        m_pRenderTarget = nullptr;
        m_bitmap = nullptr;
    }

    // Draw content.
    void OnRender() override
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

    void LoadTestBitmap()
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

    // Resize the render target.
    void OnResize(UINT width, UINT height) override
    {
        if (!m_pRenderTarget)
            return;
        // Note: This method can fail, but it's okay to ignore the
        // error here, because the error will be returned again
        // the next time EndDraw is called.
        m_pRenderTarget->Resize(D2D1::SizeU(width, height));
    }

    auto HandleMessage(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) -> LRESULT override
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

    Core::Direct2D::D2D1Factory m_pDirect2dFactory;
    Microsoft::WRL::ComPtr<ID2D1HwndRenderTarget> m_pRenderTarget;
    Microsoft::WRL::ComPtr<ID2D1Bitmap> m_bitmap;
};

extern "C" int main(int argc, char* args[]) 
try
{
    // https://docs.microsoft.com/en-us/windows/win32/api/heapapi/nf-heapapi-heapsetinformation
    if (!HeapSetInformation(nullptr, HeapEnableTerminationOnCorruption, nullptr, 0))
        throw Core::Error::Win32Error("HeapSetInformation() failed", GetLastError());

    Core::COM::COMThreadScope scope;

    DemoApp app;
    app.Initialize();
    return static_cast<int>(app.RunMessageLoop());
}
catch (const std::exception& ex)
{
    std::wcerr << ex.what() << std::endl;
    return 1;
}
