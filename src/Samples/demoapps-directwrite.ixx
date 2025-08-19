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

	private:
		Direct2D::D2D1Factory m_pDirect2dFactory;
		Win32::ComPtr<Win32::ID2D1HwndRenderTarget> m_pRenderTarget;
		Win32::ComPtr<Win32::IDWriteFactory> m_pDWriteFactory;
		Win32::ComPtr<Win32::IDWriteTextFormat> m_pTextFormat;
		Win32::ComPtr<Win32::ID2D1SolidColorBrush> m_pBlackBrush;
		std::wstring m_stringToRender = L"Hello, DirectWrite!";

		// Initialize device-independent resources.
		void CreateDeviceIndependentResources() override
		{
			m_pDirect2dFactory.Initialise(Win32::D2D1_FACTORY_TYPE::D2D1_FACTORY_TYPE_SINGLE_THREADED);

			Win32::HRESULT hr = Win32::DWriteCreateFactory(
				Win32::DWRITE_FACTORY_TYPE::DWRITE_FACTORY_TYPE_SHARED,
				__uuidof(Win32::IDWriteFactory),
				&m_pDWriteFactory
			);
			if (Win32::HrFailed(hr))
				throw Error::COMError("DWriteCreateFactory() failed", hr);

			hr = m_pDWriteFactory->CreateTextFormat(
				L"Gabriola",                // Font family name.
				nullptr,                       // Font collection (NULL sets it to use the system font collection).
				Win32::DWRITE_FONT_WEIGHT::DWRITE_FONT_WEIGHT_REGULAR,
				Win32::DWRITE_FONT_STYLE::DWRITE_FONT_STYLE_NORMAL,
				Win32::DWRITE_FONT_STRETCH::DWRITE_FONT_STRETCH_NORMAL,
				72.0f,
				L"en-us",
				&m_pTextFormat
			);
			if (Win32::HrFailed(hr))
				throw Error::COMError("CreateTextFormat() failed", hr);

			// Center align (horizontally) the text.
			hr = m_pTextFormat->SetTextAlignment(Win32::DWRITE_TEXT_ALIGNMENT::DWRITE_TEXT_ALIGNMENT_CENTER);
			if (Win32::HrFailed(hr))
				throw Error::COMError("SetTextAlignment() failed", hr);

			hr = m_pTextFormat->SetParagraphAlignment(Win32::DWRITE_PARAGRAPH_ALIGNMENT::DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
			if (Win32::HrFailed(hr))
				throw Error::COMError("SetParagraphAlignment() failed", hr);
		}

		// Initialize device-dependent resources.
		void CreateDeviceResources() override
		{
			if (m_pRenderTarget)
				return;

			Win32::RECT rc;
			Win32::GetClientRect(m_hwnd, &rc);

			// Create a Direct2D render target.
			m_pRenderTarget = m_pDirect2dFactory.CreateHwndRenderTarget(
				m_hwnd,
				rc.right - rc.left,
				rc.bottom - rc.top
			);

			Win32::HRESULT hr = m_pRenderTarget->CreateSolidColorBrush(
				D2D1::ColorF(D2D1::ColorF::Black),
				&m_pBlackBrush
			);
			if (Win32::HrFailed(hr))
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
			float dpi = static_cast<float>(Win32::GetDpiForWindow(Win32::GetDesktopWindow()));
			Win32::RECT rc;
			Win32::GetClientRect(m_hwnd, &rc);
			Win32::D2D1_RECT_F layoutRect = D2D1::RectF(
				static_cast<float>(rc.left),
				static_cast<float>(rc.top),
				static_cast<float>(rc.right - rc.left),
				static_cast<float>(rc.bottom - rc.top)
			);
			m_pRenderTarget->DrawTextW(
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

			if (Win32::HRESULT hr = m_pRenderTarget->EndDraw(); hr == Win32::D2DError::RecreateTarget)
			{
				std::wcout << L"Recreating target\n";
				DiscardDeviceResources();
			}
			else if (Win32::HrFailed(hr))
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
				case Win32::Messages::Size:
				{
					UINT width = Win32::LoWord(lParam);
					UINT height = Win32::HiWord(lParam);
					OnResize(width, height);
					return 0;
				}

				// https://docs.microsoft.com/en-us/windows/win32/gdi/wm-displaychange
				case Win32::Messages::DisplayChange:
				{
					Win32::InvalidateRect(hwnd, nullptr, false);
					return 0;
				}

				// https://docs.microsoft.com/en-us/windows/win32/gdi/wm-paint
				case Win32::Messages::Paint:
				{
					//OnRender();
					Win32::ValidateRect(hwnd, nullptr);
					return 0;
				}

				// https://docs.microsoft.com/en-us/windows/win32/winmsg/wm-destroy
				case Win32::Messages::Destroy:
				{
					Win32::PostQuitMessage(0);
					return 0;
				}

				default:
					return Win32::DefWindowProcW(hwnd, message, wParam, lParam);
			}
		}
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
