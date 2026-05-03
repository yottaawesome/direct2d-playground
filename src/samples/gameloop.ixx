export module samples:gameloop;
import std;
import shared;

export namespace GameLoop
{
	class Scene
	{
	private:
		bool Created = false;
		Shared::Ptr<D2D1::ID2D1SolidColorBrush> LightSlateGrayBrush;
		Shared::Ptr<D2D1::ID2D1SolidColorBrush> CornflowerBlueBrush;

	public:
		auto HasBeenCreated(this auto&& self) noexcept -> bool
		{
			return Created;
		}

		auto Draw(this auto&& self, D2D1::ID2D1HwndRenderTarget* renderTarget) -> Shared::HResult
		{
			if (not self.Created)
				self.CreateResources(renderTarget);
			try
			{
				renderTarget->BeginDraw();
				renderTarget->SetTransform(D2D1::Matrix3x2F::Identity());
				renderTarget->Clear(D2D1::ColorF(D2D1::ColorF::Aquamarine));
				return Shared::HResult{ renderTarget->EndDraw() };
			}
			catch (...)
			{
				renderTarget->EndDraw();
				throw;
			}
		}
		

		auto CreateResources(this auto&& self, D2D1::ID2D1HwndRenderTarget* renderTarget)
		{
			if (self.Created)
				return;

			auto hr = Shared::HResult{
			renderTarget->CreateSolidColorBrush(
				D2D1::ColorF{ D2D1::ColorF::LightSlateGray },
				self.LightSlateGrayBrush.ReleaseAndGetAddressOf()
			) };
			if (not hr)
				throw Shared::ComError{ hr, "CreateSolidColorBrush() failed [1]" };
			hr = Shared::HResult{
				renderTarget->CreateSolidColorBrush(
					D2D1::ColorF{ D2D1::ColorF::CornflowerBlue },
					self.CornflowerBlueBrush.ReleaseAndGetAddressOf()
				) };
			if (not hr)
				throw Shared::ComError{ hr, "CreateSolidColorBrush() failed [2]" };

			self.Created = true;
		}

		auto DiscardResources(this auto&& self)
		{
			self.LightSlateGrayBrush.reset();
			self.CornflowerBlueBrush.reset();
			self.Created = false;
		}
	};

	class MainApp
	{
	private:
		Shared::Timer timer;
		Shared::GameMainWindow window{
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
		Shared::GraphicsContext gfxContext{
			Shared::WindowSurface{ window.GetHandle(), window.GetDpi(), window.GetClientRect() }
		};
		Scene sceneResources;

	public:
		MainApp()
		{
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

			auto hr = self.sceneResources.Draw(self.gfxContext.GetRenderTarget());
			if (not hr)
			{
				if (hr.Code == D2D1::Error::RecreateTarget)
				{
					self.sceneResources.DiscardResources();
					self.gfxContext.DiscardResources();
					Win32::ValidateRect(self.window.GetHandle(), nullptr);
				}
				else
				{
					throw Shared::ComError{ hr, "Draw() failed" };
				}
			}
		}

		
		//
		//
		// Graphics resources
		void CreateResources(this auto&& self)
		{
			// Create a Direct2D render target.
			self.gfxContext.CreateResources();
			self.sceneResources.CreateResources(self.gfxContext.GetRenderTarget());
		}
		

		//
		//
		// Window events
		void OnResize(this auto&& self, std::uint32_t width, std::uint32_t height)
		{
			if (width == 0 or height == 0)
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
