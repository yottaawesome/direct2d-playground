module;

#include <windows.h>
#include <d2d1.h>
#include <d2d1helper.h>
#include <dwrite.h>
#include <wrl/client.h>

export module core:direct2d_d2d1factory;
import std;
import :error;

export namespace Core::Direct2D
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
			: m_factoryType(D2D1_FACTORY_TYPE::D2D1_FACTORY_TYPE_SINGLE_THREADED) 
		{}
		
		D2D1Factory(D2D1_FACTORY_TYPE factoryType)
		{
			Initialise(factoryType);
		}

		operator bool() const noexcept
		{
			return m_pDirect2dFactory != nullptr;
		}

		operator ID2D1Factory*() const noexcept
		{
			return m_pDirect2dFactory.Get();
		}

		void Close()
		{
			m_pDirect2dFactory = nullptr;
		}

		 auto CreateHwndRenderTarget(HWND hwnd, UINT32 width, UINT32 height) 
			 -> Microsoft::WRL::ComPtr<ID2D1HwndRenderTarget>
		{
			if (!hwnd)
				throw std::invalid_argument(__FUNCSIG__": hwnd is null");
			if (!m_pDirect2dFactory)
				Initialise(m_factoryType);

			D2D1_SIZE_U size = D2D1::SizeU(width, height);
			// Create a Direct2D render target.
			Microsoft::WRL::ComPtr<ID2D1HwndRenderTarget> renderTarget;
			// https://docs.microsoft.com/en-us/windows/win32/api/d2d1/nf-d2d1-id2d1factory-createhwndrendertarget(constd2d1_render_target_properties__constd2d1_hwnd_render_target_properties__id2d1hwndrendertarget)
			// https://docs.microsoft.com/en-us/windows/win32/api/d2d1/nf-d2d1-id2d1factory-createhwndrendertarget(constd2d1_render_target_properties_constd2d1_hwnd_render_target_properties_id2d1hwndrendertarget)
			HRESULT hr = m_pDirect2dFactory->CreateHwndRenderTarget(
				D2D1::RenderTargetProperties(),
				D2D1::HwndRenderTargetProperties(hwnd, size),
				&renderTarget
			);
			if (FAILED(hr))
				throw Core::Error::COMError("CreateHwndRenderTarget() failed", hr);

			return renderTarget;
		}

		void Initialise(D2D1_FACTORY_TYPE factoryType)
		{
			if (m_pDirect2dFactory)
				throw std::runtime_error("Already initialised");

			// Create a Direct2D factory.
			// https://docs.microsoft.com/en-us/windows/win32/api/d2d1/nf-d2d1-d2d1createfactory
			m_factoryType = factoryType;
			HRESULT hr = D2D1CreateFactory(
				factoryType, //D2D1_FACTORY_TYPE_SINGLE_THREADED,
				m_pDirect2dFactory.ReleaseAndGetAddressOf()
			);
			if (FAILED(hr))
				throw Error::COMError("D2D1CreateFactory() failed", hr);
		}
		
		auto GetFactoryType() const noexcept -> D2D1_FACTORY_TYPE
		{
			return m_factoryType;
		}

		auto Get() const noexcept -> Microsoft::WRL::ComPtr<ID2D1Factory>
		{
			return m_pDirect2dFactory;
		}

		auto GetRaw() const noexcept -> ID2D1Factory*
		{
			return m_pDirect2dFactory.Get();
		}

	protected:
		Microsoft::WRL::ComPtr<ID2D1Factory> m_pDirect2dFactory;
		D2D1_FACTORY_TYPE m_factoryType;
	};
}
