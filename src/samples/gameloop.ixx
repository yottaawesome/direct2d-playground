export module samples:gameloop;
import std;
import shared;

export namespace GameLoop
{
	class SceneResources
	{
	public:
		Shared::Ptr<D2D1::ID2D1SolidColorBrush> LightSlateGrayBrush;
		Shared::Ptr<D2D1::ID2D1SolidColorBrush> CornflowerBlueBrush;

		auto HasBeenCreated(this auto&& self) noexcept -> bool
		{
			return self.LightSlateGrayBrush and self.CornflowerBlueBrush;
		}

		auto CreateResources(this auto&& self, Shared::GraphicsContext& gfxContext)
		{
			if (self.HasBeenCreated())
				return;

			auto hr = Shared::HResult{ 
				gfxContext->CreateSolidColorBrush(
					D2D1::ColorF{ D2D1::ColorF::LightSlateGray },
					self.LightSlateGrayBrush.AddressOfTyped()
				)};
			if (not hr)
				throw Shared::ComError{ hr, "CreateSolidColorBrush() failed [1]" };
			hr = Shared::HResult{ 
				gfxContext->CreateSolidColorBrush(
					D2D1::ColorF{ D2D1::ColorF::CornflowerBlue },
					self.CornflowerBlueBrush.ReleaseAndGetAddressOf()
				)};
			if (not hr)
				throw Shared::ComError{ hr, "CreateSolidColorBrush() failed [2]" };
		}

		auto DiscardResources(this auto&& self)
		{
			self = {};
		}

		auto DiscardAndRecreateResources(this auto&& self, Shared::GraphicsContext& gfxContext)
		{
			self.DiscardResources();
			self.CreateResources(gfxContext);
		}
	};

	class MainApp
	{
	private:
		Shared::Timer timer;
		Shared::GameMainWindow window;
		Shared::GraphicsContext gfxContext;
		SceneResources sceneResources;

	public:
		MainApp()
		{
			window = Shared::GameMainWindow{
				[this] 
				{ 
					Tick(); // animate resizes
				}, 
				[this](std::uint32_t width, std::uint32_t height) 
				{ 
					OnResize(width, height); 
				},
				[this](std::uint32_t dpiX, const Win32::RECT& suggestedRect) 
				{ 
					OnDpiChanged(dpiX, suggestedRect); 
				}
			};
			gfxContext = Shared::GraphicsContext{
				Shared::WindowSurface{ window.GetHandle(), window.GetDpi(), window.GetClientRect() }
			};
			CreateResources();
		};

		//
		//
		// Game loop
		auto Update(this auto&& self, float dt)
		{
			// Add update code here.
		}

		auto Tick(this auto&& self)
		{
			auto dt = self.timer.Advance(); // seconds (can be decimal)
			// a debugger break or a long modal pause produces a huge first post-resume dt.
			// production apps clamp this to avoid doing a huge amount of work in the first frame after a pause.
			self.Update(std::min(dt, 0.1f));
			self.Render();
		}

		//
		//
		// Rendering
		auto Render(this auto&& self)
		{
			self.gfxContext.CreateResources();

			self.gfxContext->BeginDraw();

			if (auto hr = Shared::HResult{ self.gfxContext->EndDraw() }; 
				hr.Code == D2D1::Error::RecreateTarget)
			{
				self.DiscardResources();
				Win32::ValidateRect(self.window.GetHandle(), nullptr);
			}
			else if (not hr)
			{
				throw Shared::ComError{ hr, "EndDraw() failed" };
			}
		}

		
		//
		//
		// Graphics resources
		void CreateResources(this auto&& self)
		{
			// Create a Direct2D render target.
			self.gfxContext.CreateResources();
			self.sceneResources.CreateResources(self.gfxContext);
		}
		void DiscardResources(this auto&& self)
		{
			self.gfxContext.DiscardDeviceResources();
			self.sceneResources.DiscardResources();
		}

		

		//
		//
		// Window events
		void OnResize(this auto&& self, std::uint32_t width, std::uint32_t height)
		{
			if (not self.gfxContext.HasTarget())
				return;
			self.gfxContext.OnResize(width, height);
		}

		void OnDpiChanged(this auto&& self, std::uint32_t dpi, const Win32::RECT& rect)
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
			self.gfxContext.OnDpiChanged(dpi);
			Win32::InvalidateRect(self.window.GetHandle(), nullptr, false);
		}

		//
		//
		// Misc
		[[nodiscard]]
		auto GetWindow(this auto&& self) -> Win32::HWND
		{
			return self.window.GetHandle();
		}
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
