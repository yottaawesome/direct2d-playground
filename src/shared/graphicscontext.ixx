export module shared:graphicscontext;
import std;
import :comptr;
import :win32;
import :error;

export namespace Shared
{
	struct WindowSurface
	{
		Win32::HWND Hwnd{};
		std::uint32_t Dpi{};
		Win32::RECT Client{};
	};

	class GraphicsContext
	{
	public:
		GraphicsContext() = default;

		GraphicsContext(GraphicsContext&& other)
			: surface(std::move(other.surface)),
			direct2dFactory(std::move(other.direct2dFactory)),
			renderTarget(std::move(other.renderTarget))
		{ }
		auto operator=(GraphicsContext&& other) -> GraphicsContext&
		{
			if (this != &other)
			{
				surface = std::move(other.surface);
				direct2dFactory = std::move(other.direct2dFactory);
				renderTarget = std::move(other.renderTarget);
			}
			return *this;
		}

		GraphicsContext(WindowSurface window)
		{
			Init(window);
		}

		auto HasTarget(this auto&& self) noexcept -> bool
		{
			return self.renderTarget != nullptr;
		}

		void Init(this auto&& self, WindowSurface window)
		{
			if (self.surface.Hwnd) 
				throw Error{ "already initialized" };
			if (not window.Hwnd)
				throw Error{ "hwnd must not be null" };
			self.surface = window;
			self.CreateDeviceIndependentResources();
			self.CreateDeviceResources();
			self.CreateDpiDependentResources();
		}

		// Events
		void OnResize(this auto&& self, std::uint32_t width, std::uint32_t height)
		{
			if (not self.renderTarget)
				return;
			// Note: This method can fail, but it's okay to ignore the
			// error here, because the error will be returned again
			// the next time EndDraw is called.
			self.renderTarget->Resize(D2D1::SizeU(width, height));
		}

		void OnDpiChanged(this auto&& self, std::uint32_t dpi)
		{
			self.surface.Dpi = dpi;	
			if (self.renderTarget)
				self.renderTarget->SetDpi(static_cast<float>(dpi), static_cast<float>(dpi));
			self.DiscardDpiDependentResources();
			self.CreateDpiDependentResources();
		}

		void CreateResources(this auto&& self)
		{
			self.CreateDeviceResources();
			self.CreateDpiDependentResources();
		}


		// DPI resources
		void CreateDpiDependentResources(this auto&& self)
		{
			if (not self.renderTarget)
				throw Error{ "render target must not be null" };
			// TODO: add a check to avoid recreating resources if the resources are already created for the current DPI
		}
		void DiscardDpiDependentResources(this auto&& self)
		{}


		// Device resources
		void CreateDeviceResources(this auto&& self)
		{
			if (self.renderTarget)
				return;

			auto size = D2D1::SizeU(
				self.surface.Client.right - self.surface.Client.left, 
				self.surface.Client.bottom - self.surface.Client.top
			);

			// Create a Direct2D render target.
			auto hr = Win32::HRESULT{
				self.direct2dFactory->CreateHwndRenderTarget(
					D2D1::RenderTargetProperties(),
					D2D1::HwndRenderTargetProperties(self.surface.Hwnd, size),
					self.renderTarget.AddressOfTyped()
				) };
			if (Win32::Failed(hr))
				throw ComError{ hr, "CreateHwndRenderTarget() failed" };

			self.CreateDpiDependentResources();
		}

		void DiscardDeviceResources(this auto&& self)
		{
			self.renderTarget.reset();
			self.DiscardDpiDependentResources();
		}

		[[nodiscard]]
		auto operator->(this auto&& self) noexcept { return self.renderTarget.Get(); }
	protected:
		WindowSurface surface{};
		Ptr<D2D1::ID2D1Factory> direct2dFactory;
		Ptr<D2D1::ID2D1HwndRenderTarget> renderTarget;

		// Device independent resources
		void CreateDeviceIndependentResources(this auto&& self)
		{
			if (self.direct2dFactory)
				return;
			// Create a Direct2D factory.
			auto hr = HResult{
				D2D1::D2D1CreateFactory(
					D2D1::D2D1_FACTORY_TYPE::D2D1_FACTORY_TYPE_SINGLE_THREADED,
					self.direct2dFactory.ReleaseAndGetAddressOf()
				) };
			if (not hr)
				throw ComError{ hr, "D2D1CreateFactory() failed" };
		}
	};
}