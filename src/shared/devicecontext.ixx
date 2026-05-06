export module shared:devicecontext;
import std;
import :comptr;
import :win32;
import :error;
import :windowsurface;

export namespace Shared
{
	class DeviceContext
	{
	public:
		constexpr DeviceContext() = default;

		#pragma region Non-copyable, movable
		DeviceContext(const DeviceContext&) noexcept = delete;
		DeviceContext& operator=(const DeviceContext&) noexcept = delete;

		DeviceContext(DeviceContext&&) noexcept = default;
		DeviceContext& operator=(DeviceContext&&) noexcept = default;
		#pragma endregion

		constexpr DeviceContext(WindowSurface surface)
			: surface(surface)
		{
			if (not surface.Hwnd)
				throw Error{ "Invalid window handle in DeviceContext constructor" };

			CreateResources();
		}

		void OnResize(this auto&& self, std::uint32_t width, std::uint32_t height)
		{
			if (not self.swapChain)
				return;
			self.d2dDeviceContext->SetTarget(nullptr);
			self.targetBitmap.reset();
			auto hr = HResult{
				self.swapChain->ResizeBuffers(
					0,
					width,
					height,
					DXGI_FORMAT_UNKNOWN,
					0
				) };
			if (not hr)
				throw ComError{ hr, "Failed to resize swap chain buffers" };
			self.CreateTargetBitmap();
		}

		#pragma region Rendering
		void BeginDraw(this auto&& self)
		{
			self.d2dDeviceContext->BeginDraw();
		}

		auto EndDraw(this auto&& self) -> Shared::HResult
		{
			return Shared::HResult{ self.d2dDeviceContext->EndDraw() };
		}

		// https://learn.microsoft.com/en-us/windows/win32/api/dxgi/nf-dxgi-idxgiswapchain-present
		auto Present(
			this auto&& self, 
			std::uint32_t syncInterval, 
			std::uint32_t flags
		) -> Shared::HResult
		{
			return Shared::HResult{ self.swapChain->Present(syncInterval, flags) };
		}
		#pragma endregion

		#pragma region Getters
		[[nodiscard]]
		constexpr auto operator->(this auto&& self) noexcept
		{
			return self.d2dDeviceContext.Get();
		}

		[[nodiscard]]
		constexpr auto GetDevice(this auto&& self) noexcept -> D3D11::ID3D11Device*
		{
			return self.d3dDevice.Get();
		}
		
		[[nodiscard]]
		constexpr auto GetDeviceContext(this auto&& self) noexcept -> D3D11::ID3D11DeviceContext*
		{
			return self.deviceContext.Get();
		}

		[[nodiscard]]
		consteval auto GetCreationFlags(this auto&& self) noexcept -> unsigned int
		{
			return D3D11::D3D11_CREATE_DEVICE_FLAG::D3D11_CREATE_DEVICE_BGRA_SUPPORT;
		}

		[[nodiscard]]
		consteval auto GetFeatureLevels(this auto&& self) noexcept -> auto
		{
			return std::array{
				D3D11::D3D_FEATURE_LEVEL::D3D_FEATURE_LEVEL_11_1,
				D3D11::D3D_FEATURE_LEVEL::D3D_FEATURE_LEVEL_11_0
			};
		}
		#pragma endregion

		#pragma region Resource management
		auto DiscardResources(this auto&& self)
		{
			if (self.d2dDeviceContext)
				self.d2dDeviceContext->SetTarget(nullptr);
			self.targetBitmap.reset();
			self.swapChain.reset();
			self.d2dDeviceContext.reset();
			self.d2dDevice.reset();
			self.dxgiDevice.reset();
			self.d3d11DeviceContext.reset();
			self.d3d11Device.reset();
		}
		auto CreateResources(this auto&& self)
		{
			if (self.d2dDeviceContext)
				return;
			self.CreateDeviceResources();
			self.CreateSwapChain();
			self.CreateTargetBitmap();
		}
		#pragma endregion

	private:
		void CreateDeviceResources(this auto&& self)
		{
			// Create a Direct2D factory.
			auto hr = HResult{
				D2D1::D2D1CreateFactory(
					D2D1::D2D1_FACTORY_TYPE::D2D1_FACTORY_TYPE_SINGLE_THREADED,
					self.d2dFactory.ReleaseAndGetAddressOf()
				) };
			if (not hr)
				throw ComError{ hr, "D2D1CreateFactory() failed" };

			// Need a ID2D1Factory1 to create a ID2D1Device.
			self.d2dFactory1 = self.d2dFactory.As<D2D1::ID2D1Factory1>();

			constexpr auto creationFlags = self.GetCreationFlags();
			constexpr auto featureLevels = self.GetFeatureLevels();

			// https://learn.microsoft.com/en-us/windows/win32/api/d3d11/nf-d3d11-d3d11createdevice
			auto featureLevel = D3D11::D3D_FEATURE_LEVEL{};
			hr = HResult{
				D3D11::D3D11CreateDevice(
					nullptr,
					D3D11::D3D_DRIVER_TYPE::D3D_DRIVER_TYPE_HARDWARE,
					0,
					creationFlags,
					featureLevels.data(),
					static_cast<unsigned int>(featureLevels.size()),
					D3D11::SdkVersion,
					self.d3d11Device.ReleaseAndGetAddressOf(),
					&featureLevel,
					self.d3d11DeviceContext.ReleaseAndGetAddressOf()
				) };
			if (not hr)
				throw ComError{ hr, "Failed to create D3D11 device and context" };

			self.dxgiDevice = self.d3d11Device.As<DXGI::IDXGIDevice>();

			hr = HResult{
				self.d2dFactory1->CreateDevice(
					self.dxgiDevice.Get(),
					self.d2dDevice.ReleaseAndGetAddressOf()
				) };
			if (not hr)
				throw ComError{ hr, "Failed to create D2D1 device" };

			hr = HResult{
				self.d2dDevice->CreateDeviceContext(
					D2D1::D2D1_DEVICE_CONTEXT_OPTIONS::D2D1_DEVICE_CONTEXT_OPTIONS_NONE,
					self.d2dDeviceContext.ReleaseAndGetAddressOf()
				) };
			if (not hr)
				throw ComError{ hr, "Failed to create D2D1 device context" };
		}

		void CreateSwapChain(this auto&& self)
		{
			auto adapter = Shared::Ptr<DXGI::IDXGIAdapter>{};
			auto hr = Shared::HResult{ self.dxgiDevice->GetAdapter(adapter.ReleaseAndGetAddressOf()) };
			if (not hr)
				throw Shared::ComError{ hr, "IDXGIDevice::GetAdapter() failed" };

			auto factory = Shared::Ptr<DXGI::IDXGIFactory2>{};
			hr = adapter->GetParent(
				factory.GetUuid(),
				reinterpret_cast<void**>(factory.ReleaseAndGetAddressOf())
			);
			if (not hr)
				throw Shared::ComError{ hr, "IDXGIAdapter::GetParent() failed" };

			auto swapChainDesc = DXGI::DXGI_SWAP_CHAIN_DESC1{
				.Width = 0,
				.Height = 0,
				.Format = DXGI::DXGI_FORMAT::DXGI_FORMAT_B8G8R8A8_UNORM,
				.Stereo = false,
				.SampleDesc = DXGI::DXGI_SAMPLE_DESC{.Count = 1, .Quality = 0 },
				.BufferUsage = DXGI::Usage::RenderTargetOutput,
				.BufferCount = 2,
				.Scaling = DXGI::DXGI_SCALING::DXGI_SCALING_STRETCH,
				.SwapEffect = DXGI::DXGI_SWAP_EFFECT::DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL,
				.AlphaMode = DXGI::DXGI_ALPHA_MODE::DXGI_ALPHA_MODE_IGNORE,
				.Flags = 0
			};

			hr = factory->CreateSwapChainForHwnd(
				self.d3d11Device.Get(),
				self.surface.Hwnd,
				&swapChainDesc,
				nullptr,
				nullptr,
				self.swapChain.ReleaseAndGetAddressOf()
			);
			if (not hr)
				throw Shared::ComError{ hr, "IDXGIFactory2::CreateSwapChainForHwnd() failed" };
		}

		void CreateTargetBitmap(this auto&& self)
		{
			auto backBuffer = Shared::Ptr<DXGI::IDXGISurface>{};
			auto hr = Shared::HResult{ 
				self.swapChain->GetBuffer(
					0,
					backBuffer.GetUuid(),
					reinterpret_cast<void**>(backBuffer.ReleaseAndGetAddressOf())
				) };
			if (not hr)
				throw Shared::ComError{ hr, "IDXGISwapChain::GetBuffer() failed" };

			auto bitmapProperties =
				D2D1::D2D1_BITMAP_PROPERTIES1{
					.pixelFormat = D2D1::D2D1_PIXEL_FORMAT{
						.format = DXGI::DXGI_FORMAT::DXGI_FORMAT_B8G8R8A8_UNORM,
						.alphaMode = D2D1::D2D1_ALPHA_MODE::D2D1_ALPHA_MODE_PREMULTIPLIED
					},
					.dpiX = static_cast<float>(self.surface.GetDpi()),
					.dpiY = static_cast<float>(self.surface.GetDpi()),
					.bitmapOptions = static_cast<D2D1::D2D1_BITMAP_OPTIONS>(D2D1::D2D1_BITMAP_OPTIONS::D2D1_BITMAP_OPTIONS_TARGET | D2D1::D2D1_BITMAP_OPTIONS::D2D1_BITMAP_OPTIONS_CANNOT_DRAW),
					.colorContext = nullptr
				};
			hr = self.d2dDeviceContext->CreateBitmapFromDxgiSurface(
				backBuffer.Get(),
				&bitmapProperties,
				self.targetBitmap.ReleaseAndGetAddressOf()
			);
			if (not hr)
				throw ComError{ hr, "ID2D1DeviceContext::CreateBitmapFromDxgiSurface() failed" };

			self.d2dDeviceContext->SetTarget(self.targetBitmap.Get());
		}

		Ptr<D3D11::ID3D11Device> d3d11Device;
		Ptr<D3D11::ID3D11DeviceContext> d3d11DeviceContext;
		Ptr<DXGI::IDXGIDevice> dxgiDevice;
		Ptr<D2D1::ID2D1Factory> d2dFactory;
		Ptr<D2D1::ID2D1Factory1> d2dFactory1;
		Ptr<D2D1::ID2D1Device> d2dDevice;
		Ptr<D2D1::ID2D1DeviceContext> d2dDeviceContext;
		Ptr<DXGI::IDXGISwapChain1> swapChain;
		Ptr<D2D1::ID2D1Bitmap1> targetBitmap;
		WindowSurface surface;
	};
}
