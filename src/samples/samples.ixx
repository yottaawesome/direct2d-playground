export module samples;
import std;
import shared;

// Draws a grid and two rectangles (one filled, one outlined) using Direct2D.
// This is an adapted version of the Direct2D "Rectangles" sample, located at 
// https://learn.microsoft.com/en-us/windows/win32/direct2d/direct2d-quickstart. 
// The original sample is licensed under the MIT License.
export namespace Rectangles
{
	class SampleApp final : public Shared::Window<SampleApp>
	{
	public:
		SampleApp()
		{
			CreateDeviceIndependentResources();
			CreateDeviceResources();
		};

		auto CreateDeviceIndependentResources(this auto&& self)
		{
			// Create a Direct2D factory.
			auto hr = Shared::HResult{
				D2D1::D2D1CreateFactory(
					D2D1::D2D1_FACTORY_TYPE::D2D1_FACTORY_TYPE_SINGLE_THREADED, 
					self.direct2dFactory.ReleaseAndGetAddressOf()
				)};
			if (not hr)
				throw Shared::ComError{ hr, "D2D1CreateFactory() failed" };
		}

		auto CreateDeviceResources(this auto&& self)
		{
			if (self.renderTarget)
				return;

			auto rc = self.GetClientRect();

			auto size = D2D1::SizeU(rc.right - rc.left, rc.bottom - rc.top);

			// Create a Direct2D render target.
			auto hr = Win32::HRESULT{
				self.direct2dFactory->CreateHwndRenderTarget(
					D2D1::RenderTargetProperties(),
					D2D1::HwndRenderTargetProperties(self.GetHandle(), size),
					self.renderTarget.AddressOfTyped()
				) };
			if (Win32::Failed(hr))
				throw Shared::ComError{ hr, "CreateHwndRenderTarget() failed" };

			// Create a gray brush.
			hr = self.renderTarget->CreateSolidColorBrush(
				D2D1::ColorF{ D2D1::ColorF::LightSlateGray },
				self.lightSlateGrayBrush.AddressOfTyped()
			);
			if (Win32::Failed(hr))
				throw Shared::ComError{ hr, "CreateSolidColorBrush() failed [1]" };

			// Create a blue brush.
			hr = self.renderTarget->CreateSolidColorBrush(
				D2D1::ColorF{ D2D1::ColorF::CornflowerBlue },
				self.cornflowerBlueBrush.ReleaseAndGetAddressOf()
			);
			if (Win32::Failed(hr))
				throw Shared::ComError{ hr, "CreateSolidColorBrush() failed [2]" };
		}

		void DiscardDeviceResources(this auto&& self)
		{
			self.renderTarget.reset();
			self.lightSlateGrayBrush.reset();
			self.cornflowerBlueBrush.reset();
		}

		auto OnMessage(this auto&& self, const Shared::Messages::Paint& msg) -> Win32::LRESULT
		{
			self.OnRender();
			Win32::ValidateRect(self.GetHandle(), nullptr);
			return 0;
		}

		auto OnMessage(this auto&& self, const Shared::Messages::Size& msg) -> Win32::LRESULT
		{
			if (not self.renderTarget)
				return 0;
			self.OnResize(Win32::LoWord(msg.LParam), Win32::HiWord(msg.LParam));
			return 0;
		}

		auto OnMessage(this auto&& self, const Shared::Messages::DisplayChange& msg) -> Win32::LRESULT
		{
			Win32::InvalidateRect(self.GetHandle(), nullptr, false);
			return 0;
		}

		void OnResize(this auto&& self, Win32::UINT width, Win32::UINT height)
		{
			if (not self.renderTarget)
				return;
			// Note: This method can fail, but it's okay to ignore the
			// error here, because the error will be returned again
			// the next time EndDraw is called.
			self.renderTarget->Resize(D2D1::SizeU(width, height));
		}

		auto OnRender(this auto&& self)
		{
			self.CreateDeviceResources();

			self.renderTarget->BeginDraw();
			self.renderTarget->SetTransform(D2D1::Matrix3x2F::Identity());
			self.renderTarget->Clear(D2D1::ColorF(D2D1::ColorF::White));

			// Draw a grid background.
			auto rtSize = D2D1::D2D1_SIZE_F{ self.renderTarget->GetSize() };
			auto width = static_cast<int>(rtSize.width);
			auto height = static_cast<int>(rtSize.height);
			for (int x = 0; x < width; x += 10)
			{
				self.renderTarget->DrawLine(
					D2D1::Point2F(static_cast<FLOAT>(x), 0.0f),
					D2D1::Point2F(static_cast<FLOAT>(x), rtSize.height),
					self.lightSlateGrayBrush.Get(),
					0.5f
				);
			}
			for (int y = 0; y < height; y += 10)
			{
				self.renderTarget->DrawLine(
					D2D1::Point2F(0.0f, static_cast<FLOAT>(y)),
					D2D1::Point2F(rtSize.width, static_cast<FLOAT>(y)),
					self.lightSlateGrayBrush.Get(),
					0.5f
				);
			}

			// Draw a filled rectangle.
			auto rectangle1 = D2D1::D2D1_RECT_F{
				D2D1::RectF(
					rtSize.width / 2 - 50.0f,
					rtSize.height / 2 - 50.0f,
					rtSize.width / 2 + 50.0f,
					rtSize.height / 2 + 50.0f
				) };
			self.renderTarget->FillRectangle(&rectangle1, self.lightSlateGrayBrush.Get());

			// Draw the outline of a rectangle.
			auto rectangle2 = D2D1_RECT_F{
				D2D1::RectF(
					rtSize.width / 2 - 100.0f,
					rtSize.height / 2 - 100.0f,
					rtSize.width / 2 + 100.0f,
					rtSize.height / 2 + 100.0f
				) };
			self.renderTarget->DrawRectangle(&rectangle2, self.cornflowerBlueBrush.Get());

			auto hr = Win32::HRESULT{ self.renderTarget->EndDraw() };
			if (Win32::Failed(hr))
			{
				if (hr != D2D1::Error::RecreateTarget)
					throw Shared::Win32Error(hr, "EndDraw() failed");
				self.DiscardDeviceResources();
				Win32::ValidateRect(self.GetHandle(), nullptr);
			}
		}

	private:
		Shared::Ptr<D2D1::ID2D1Factory> direct2dFactory;
		Shared::Ptr<D2D1::ID2D1HwndRenderTarget> renderTarget;
		Shared::Ptr<D2D1::ID2D1SolidColorBrush> lightSlateGrayBrush;
		Shared::Ptr<D2D1::ID2D1SolidColorBrush> cornflowerBlueBrush;
	};

	void Run()
	{
		Win32::CoInitializeEx(nullptr, Win32::COINIT::COINIT_MULTITHREADED | Win32::COINIT::COINIT_DISABLE_OLE1DDE);
		auto window = SampleApp{};
		auto msg = Win32::MSG{};
		while (msg.message != Win32::Messages::Quit)
		{
			if (Win32::PeekMessageW(&msg, window.GetHandle(), 0, 0, Win32::PeekMessageFlags::Remove))
			{
				Win32::TranslateMessage(&msg);
				Win32::DispatchMessageW(&msg);
			}
			else
			{
				// idle, add processing here
			}
		}
	}
}