module;

#include <windows.h>
#include <d2d1.h>
#include <dwrite.h>
#include <wrl/client.h>

export module demoapps:directwrite;
import std;
import core;

namespace DirectWrite
{
	// This is based on https://docs.microsoft.com/en-us/windows/win32/directwrite/getting-started-with-directwrite
	struct DemoApp final : UI::MainWindow2
	{
		~DemoApp()
		{
			m_pDirect2dFactory.Close();
			m_pRenderTarget = nullptr;
		}

		DemoApp()
			: UI::MainWindow2(),
			m_stringToRender(L"Hello, DirectWrite!")
		{ }

	private:
		// Initialize device-independent resources.
		void CreateDeviceIndependentResources() override
		{
			m_pDirect2dFactory.Initialise(D2D1_FACTORY_TYPE_SINGLE_THREADED);

			HRESULT hr = DWriteCreateFactory(
				DWRITE_FACTORY_TYPE_SHARED,
				__uuidof(IDWriteFactory),
				&m_pDWriteFactory
			);
			if (FAILED(hr))
				throw Error::COMError("DWriteCreateFactory() failed", hr);

			hr = m_pDWriteFactory->CreateTextFormat(
				L"Gabriola",                // Font family name.
				NULL,                       // Font collection (NULL sets it to use the system font collection).
				DWRITE_FONT_WEIGHT_REGULAR,
				DWRITE_FONT_STYLE_NORMAL,
				DWRITE_FONT_STRETCH_NORMAL,
				72.0f,
				L"en-us",
				&m_pTextFormat
			);
			if (FAILED(hr))
				throw Error::COMError("CreateTextFormat() failed", hr);

			// Center align (horizontally) the text.
			hr = m_pTextFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
			if (FAILED(hr))
				throw Error::COMError("SetTextAlignment() failed", hr);

			hr = m_pTextFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
			if (FAILED(hr))
				throw Error::COMError("SetParagraphAlignment() failed", hr);
		}

		// Initialize device-dependent resources.
		void CreateDeviceResources() override
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
				&m_pBlackBrush
			);
			if (FAILED(hr))
				throw Error::COMError("SetParagraphAlignment() failed", hr);
		}

		// Release device-dependent resource.
		void DiscardDeviceResources() override
		{
			m_pRenderTarget = nullptr;
			m_pBlackBrush = nullptr;
		}

		void DrawText()
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
				m_pTextFormat.Get(),    // The text format.
				layoutRect,       // The region of the window where the text will be rendered.
				m_pBlackBrush.Get()     // The brush used to draw the text.
			);
		}

		// Draw content.
		void OnRender() override
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
				throw Error::COMError("EndDraw() failed", hr);
			}
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

	protected:
		Direct2D::D2D1Factory m_pDirect2dFactory;
		Microsoft::WRL::ComPtr<ID2D1HwndRenderTarget> m_pRenderTarget;
		Microsoft::WRL::ComPtr<IDWriteFactory> m_pDWriteFactory;
		Microsoft::WRL::ComPtr<IDWriteTextFormat> m_pTextFormat;
		Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> m_pBlackBrush;
		std::wstring m_stringToRender;
	};
}

export namespace DirectWrite
{
	auto Run() -> int
	{
		COM::COMThreadScope scope;
		DemoApp app;
		app.Initialize();
		return static_cast<int>(app.RunMessageLoop());
	}
}
