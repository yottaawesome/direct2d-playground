export module samples:gameloop;
import std;
import shared;

export namespace GameLoop
{
	class MainWindow final : public Shared::Window<MainWindow>
	{
	public:
		using RenderFn = std::move_only_function<auto()->void>;
		using ResizeFn = std::move_only_function<auto(std::uint32_t, std::uint32_t)->void>;
		using DpiChangedFn = std::move_only_function<auto(std::uint32_t, const Win32::RECT&)->void>;

		RenderFn OnRender = [] {};
		ResizeFn OnResize = [](std::uint32_t, std::uint32_t) {};
		DpiChangedFn OnDpiChanged = [](std::uint32_t, const Win32::RECT&) {};

		MainWindow(RenderFn onRender, ResizeFn onResize, DpiChangedFn onDpiChanged)
			: Shared::Window<MainWindow>{ false }
			, OnRender(std::move(onRender))
			, OnResize(std::move(onResize))
			, OnDpiChanged(std::move(onDpiChanged))
		{
			// We need to Init() after setting the function objects, 
			// because the message handlers call those functions.
			// Letting the base constructor do the init would cause 
			// the handlers to call empty function objects, generating 
			// errors.
			Init();
		}

		// This is the minimum message handling needed to support rendering in the main loop. 
		// You can add handlers for other messages (e.g. WM_SIZE) as needed.
		auto OnMessage(this auto&& self, const Shared::Messages::Paint& msg) -> Win32::LRESULT
		{
			auto ps = Win32::PAINTSTRUCT{};
			Win32::BeginPaint(self.GetHandle(), &ps);
			self.OnRender();
			Win32::EndPaint(self.GetHandle(), &ps);
			return 0;
		}
		auto OnMessage(this auto&& self, const Shared::Messages::Size& msg) -> Win32::LRESULT
		{
			auto width = Win32::LoWord(msg.LParam);
			auto height = Win32::HiWord(msg.LParam);
			self.OnResize(width, height);
			return 0;
		}
		auto OnMessage(this auto&& self, const Shared::Messages::DisplayChange& msg) -> Win32::LRESULT
		{
			const auto dpi = std::uint32_t{ Win32::HiWord(msg.WParam) };
			const auto rect = reinterpret_cast<const Win32::RECT*>(msg.LParam);
			self.OnDpiChanged(dpi, *rect);
			return 0;
		}
		auto OnMessage(this auto&& self, const Shared::Messages::DpiChanged& msg) -> Win32::LRESULT
		{
			Win32::InvalidateRect(self.GetHandle(), nullptr, false);
			return 0;
		}
	};

	class MainApp
	{
	public:
		MainApp()
		{
			CreateDeviceIndependentResources();
			CreateDeviceResources();
		};

		auto Update(this auto&& self, float dt)
		{
			// Add update code here.
		}
		auto Render(this auto&& self)
		{
			self.CreateDeviceResources();

			self.renderTarget->BeginDraw();

			auto hr = Shared::HResult{ self.renderTarget->EndDraw() };
			if (hr == Shared::HResult{ D2D1::Error::RecreateTarget })
			{
				self.DiscardDeviceResources();
				Win32::ValidateRect(self.window.GetHandle(), nullptr);
			}
			else if (not hr)
			{
				throw Shared::ComError{ hr, "EndDraw() failed" };
			}
		}
		auto GetWindow(this auto&& self) -> Win32::HWND
		{
			return self.window.GetHandle();
		}

		auto CreateDeviceIndependentResources(this auto&& self)
		{
			// Create a Direct2D factory.
			auto hr = Shared::HResult{
				D2D1::D2D1CreateFactory(
					D2D1::D2D1_FACTORY_TYPE::D2D1_FACTORY_TYPE_SINGLE_THREADED,
					self.direct2dFactory.ReleaseAndGetAddressOf()
				) };
			if (not hr)
				throw Shared::ComError{ hr, "D2D1CreateFactory() failed" };
		}
		auto CreateDeviceResources(this auto&& self)
		{
			if (self.renderTarget)
				return;

			auto rc = self.window.GetClientRect();

			auto size = D2D1::SizeU(rc.right - rc.left, rc.bottom - rc.top);

			// Create a Direct2D render target.
			auto hr = Win32::HRESULT{
				self.direct2dFactory->CreateHwndRenderTarget(
					D2D1::RenderTargetProperties(),
					D2D1::HwndRenderTargetProperties(self.window.GetHandle(), size),
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
		void ReleaseDpiDependentResources() {}
		void CreateDpiDependentResources(std::uint32_t dpi) {}

		auto Tick(this auto&& self)
		{
			const auto now = std::chrono::steady_clock::now();
			auto dt = std::chrono::duration<float>(now - self.last).count(); // seconds (can be decimal)
			self.last = now;
			// a debugger break or a long modal pause produces a huge first post-resume dt.
			// production apps clamp this to avoid doing a huge amount of work in the first frame after a pause.
			self.Update(std::min(dt, 0.1f));
			self.Render();
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

		void OnDpiChanged(this auto&& self, Win32::UINT dpi, const Win32::RECT& rect)
		{
			Win32::SetWindowPos(
				self.window.GetHandle(),
				nullptr,
				rect.left,
				rect.top,
				rect.right - rect.left,
				rect.bottom - rect.top,
				Win32::SetWindowPosFlags::NoZOrder | Win32::SetWindowPosFlags::NoActivate
			);
			if (self.renderTarget)
				self.renderTarget->SetDpi(static_cast<float>(dpi), static_cast<float>(dpi));

			self.ReleaseDpiDependentResources();
			self.CreateDpiDependentResources(dpi);
			Win32::InvalidateRect(self.window.GetHandle(), nullptr, false);
		}

	private:
		MainWindow window{
			[this] { Tick(); }, // animate resizes
			[this](std::uint32_t width, std::uint32_t height) { OnResize(width, height); },
			[this](std::uint32_t dpiX, const Win32::RECT& suggestedRect) { OnDpiChanged(dpiX, suggestedRect); }
		};
		std::chrono::steady_clock::time_point last = std::chrono::steady_clock::now();

		Shared::Ptr<D2D1::ID2D1Factory> direct2dFactory;
		Shared::Ptr<D2D1::ID2D1HwndRenderTarget> renderTarget;
		Shared::Ptr<D2D1::ID2D1SolidColorBrush> lightSlateGrayBrush;
		Shared::Ptr<D2D1::ID2D1SolidColorBrush> cornflowerBlueBrush;
	};

	void Run()
	{
		auto app = MainApp{};
		auto msg = Win32::MSG{};

		while (msg.message != Win32::Messages::Quit)
		{
			if (Win32::PeekMessageW(&msg, nullptr, 0, 0, Win32::PeekMessageFlags::Remove))
			{
				Win32::TranslateMessage(&msg);
				Win32::DispatchMessageW(&msg);
			}
			else
			{
				app.Tick();
			}
		}
	}
}
