export module shared:devicecontext;
import std;
import :comptr;
import :win32;
import :error;

export namespace Shared
{
	class DeviceContext
	{
	public:
		DeviceContext(bool init)
		{
			init ? Initialise() : void();
		}

		constexpr auto operator->(this auto&& self) noexcept
		{
			return self.deviceContext.Get();
		}

		constexpr auto GetDevice(this auto&& self) noexcept
		{
			return self.device.Get();
		}
		
		constexpr auto GetDeviceContext(this auto&& self) noexcept
		{
			return self.deviceContext.Get();
		}

		consteval auto GetCreationFlags(this auto&& self) noexcept
		{
			return D3D11::D3D11_CREATE_DEVICE_FLAG::D3D11_CREATE_DEVICE_BGRA_SUPPORT;
		}

		consteval auto GetFeatureLevels(this auto&& self) noexcept
		{
			return std::array{
				D3D11::D3D_FEATURE_LEVEL::D3D_FEATURE_LEVEL_11_1,
				D3D11::D3D_FEATURE_LEVEL::D3D_FEATURE_LEVEL_11_0
			};
		}
	private:
		void Initialise(this auto& self)
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
					self.device.ReleaseAndGetAddressOf(),
					&featureLevel,
					self.deviceContext.ReleaseAndGetAddressOf()
				) };
			if (not hr)
				throw ComError{ hr, "Failed to create D3D11 device and context" };

			self.dxgiDevice = self.device.As<DXGI::IDXGIDevice>();

			hr = HResult{
				self.d2dFactory1->CreateDevice(
					self.dxgiDevice.Get(), 
					self.d2dDevice.ReleaseAndGetAddressOf()
				)};
			if(not hr)
				throw ComError{ hr, "Failed to create D2D1 device" };

			hr = HResult{
				self.d2dDevice->CreateDeviceContext(
					D2D1::D2D1_DEVICE_CONTEXT_OPTIONS::D2D1_DEVICE_CONTEXT_OPTIONS_NONE,
					self.d2dContext.ReleaseAndGetAddressOf()
				) };
			if (not hr)
				throw ComError{ hr, "Failed to create D2D1 device context" };
		}

		Ptr<D3D11::ID3D11Device> device;
		Ptr<D3D11::ID3D11DeviceContext> deviceContext;
		Ptr<DXGI::IDXGIDevice> dxgiDevice;
		Ptr<D2D1::ID2D1Factory> d2dFactory;
		Ptr<D2D1::ID2D1Factory1> d2dFactory1;
		Ptr<D2D1::ID2D1Device> d2dDevice;
		Ptr<D2D1::ID2D1DeviceContext> d2dContext;
	};
}
