export module core:direct2d_d2d1factory;
import std;
import :win32;
import :error;

export namespace Direct2D
{
	// Represents an ID2D1Factory. See also https://docs.microsoft.com/en-us/windows/win32/api/d2d1/nn-d2d1-id2d1factory
	// Make sure to link to #pragma comment(lib, "d2d1.lib")
	class D2D1Factory final
	{
	public:
		~D2D1Factory()
		{
			Close();
		}
		
		D2D1Factory() 
			: m_factoryType(Win32::D2D1_FACTORY_TYPE::D2D1_FACTORY_TYPE_SINGLE_THREADED) 
		{}
		
		D2D1Factory(Win32::D2D1_FACTORY_TYPE factoryType)
		{
			Initialise(factoryType);
		}

		operator bool() const noexcept
		{
			return m_pDirect2dFactory != nullptr;
		}

		operator Win32::ID2D1Factory*() const noexcept
		{
			return m_pDirect2dFactory.Get();
		}

		void Close()
		{
			m_pDirect2dFactory = nullptr;
		}

		 auto CreateHwndRenderTarget(Win32::HWND hwnd, Win32::UINT32 width, Win32::UINT32 height)
			 -> Win32::ComPtr<Win32::ID2D1HwndRenderTarget>
		{
			if (!hwnd)
				throw std::invalid_argument("hwnd is null");
			if (!m_pDirect2dFactory)
				Initialise(m_factoryType);

			Win32::D2D1_SIZE_U size = D2D1::SizeU(width, height);
			// Create a Direct2D render target.
			Win32::ComPtr<Win32::ID2D1HwndRenderTarget> renderTarget;
			// https://docs.microsoft.com/en-us/windows/win32/api/d2d1/nf-d2d1-id2d1factory-createhwndrendertarget(constd2d1_render_target_properties__constd2d1_hwnd_render_target_properties__id2d1hwndrendertarget)
			// https://docs.microsoft.com/en-us/windows/win32/api/d2d1/nf-d2d1-id2d1factory-createhwndrendertarget(constd2d1_render_target_properties_constd2d1_hwnd_render_target_properties_id2d1hwndrendertarget)
			Win32::HRESULT hr = m_pDirect2dFactory->CreateHwndRenderTarget(
				D2D1::RenderTargetProperties(),
				D2D1::HwndRenderTargetProperties(hwnd, size),
				&renderTarget
			);
			if (Win32::HrFailed(hr))
				throw Error::COMError("CreateHwndRenderTarget() failed", hr);

			return renderTarget;
		}

		void Initialise(Win32::D2D1_FACTORY_TYPE factoryType)
		{
			if (m_pDirect2dFactory)
				throw std::runtime_error("Already initialised");

			// Create a Direct2D factory.
			// https://docs.microsoft.com/en-us/windows/win32/api/d2d1/nf-d2d1-d2d1createfactory
			m_factoryType = factoryType;
			Win32::HRESULT hr = Win32::D2D1CreateFactory(
				factoryType, //D2D1_FACTORY_TYPE_SINGLE_THREADED,
				m_pDirect2dFactory.ReleaseAndGetAddressOf()
			);
			if (Win32::HrFailed(hr))
				throw Error::COMError("D2D1CreateFactory() failed", hr);
		}
		
		auto GetFactoryType() const noexcept -> Win32::D2D1_FACTORY_TYPE
		{
			return m_factoryType;
		}

		auto Get() const noexcept -> Win32::ComPtr<Win32::ID2D1Factory>
		{
			return m_pDirect2dFactory;
		}

		auto GetRaw() const noexcept -> ID2D1Factory*
		{
			return m_pDirect2dFactory.Get();
		}

	protected:
		Win32::ComPtr<Win32::ID2D1Factory> m_pDirect2dFactory;
		Win32::D2D1_FACTORY_TYPE m_factoryType;
	};
}
