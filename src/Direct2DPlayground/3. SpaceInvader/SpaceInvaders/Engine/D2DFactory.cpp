module;

#include <stdexcept>
#include <source_location>
#include <string>
#include <windows.h>
#include <d2d1.h>
#include <d2d1helper.h>
#include <dwrite.h>
#include <wincodec.h>
#include <wrl/client.h>

module spaceinvaders.engine.d2dfactory;
import core.error;
import core.wic.wicimagingfactory;

namespace SpaceInvaders::Engine
{
	D2DFactory::~D2DFactory()
	{
		Close();
	}
	
	D2DFactory::D2DFactory() 
		: m_factoryType(D2D1_FACTORY_TYPE::D2D1_FACTORY_TYPE_SINGLE_THREADED)
	{}

	D2DFactory::D2DFactory(const D2D1_FACTORY_TYPE factoryType)
	{
		Initialise(factoryType);
	}

	D2DFactory::operator bool() const noexcept
	{
		return m_D2DFactory != nullptr;
	}

	void D2DFactory::Close()
	{
		m_D2DFactory = nullptr;
	}

	D2DFactory::operator ID2D1Factory* () const noexcept
	{
		return m_D2DFactory.Get();
	}

	Renderer D2DFactory::BindRenderTarget(
		const HWND hwnd,
		const UINT32 width,
		const UINT32 height
	)
	{
		if (!hwnd)
			throw std::invalid_argument(__FUNCSIG__": hwnd is null");
		if (!m_D2DFactory)
			Initialise(m_factoryType);

		const D2D1_SIZE_U size = D2D1::SizeU(width, height);
		// Create a Direct2D render target.
		Microsoft::WRL::ComPtr<ID2D1HwndRenderTarget> renderTarget;
		// https://docs.microsoft.com/en-us/windows/win32/api/d2d1/nf-d2d1-id2d1factory-createhwndrendertarget(constd2d1_render_target_properties__constd2d1_hwnd_render_target_properties__id2d1hwndrendertarget)
		// https://docs.microsoft.com/en-us/windows/win32/api/d2d1/nf-d2d1-id2d1factory-createhwndrendertarget(constd2d1_render_target_properties_constd2d1_hwnd_render_target_properties_id2d1hwndrendertarget)
		const HRESULT hr = m_D2DFactory->CreateHwndRenderTarget(
			// https://docs.microsoft.com/en-us/windows/win32/api/d2d1/ns-d2d1-d2d1_render_target_properties
			D2D1::RenderTargetProperties(),
			// https://docs.microsoft.com/en-us/windows/win32/api/d2d1/ne-d2d1-d2d1_present_options
			D2D1::HwndRenderTargetProperties(
				hwnd,
				size,
				D2D1_PRESENT_OPTIONS::D2D1_PRESENT_OPTIONS_NONE 
				// Use D2D1_PRESENT_OPTIONS_IMMEDIATELY to decouple from
				// monitor's refresh rate.
				// See https://docs.microsoft.com/en-us/windows/win32/api/d2d1/ne-d2d1-d2d1_present_options
			),
			&renderTarget
		);
		if (FAILED(hr))
			throw Core::Error::COMError(__FUNCSIG__": CreateHwndRenderTarget() failed", hr);
		return Renderer(renderTarget);
	}

	void D2DFactory::Initialise(const D2D1_FACTORY_TYPE factoryType)
	{
		if (m_D2DFactory)
			throw std::runtime_error(__FUNCSIG__": already initialised");

		// Create a Direct2D factory.
		// https://docs.microsoft.com/en-us/windows/win32/api/d2d1/nf-d2d1-d2d1createfactory
		m_factoryType = factoryType;
		const HRESULT hr = D2D1CreateFactory(
			factoryType, //D2D1_FACTORY_TYPE_SINGLE_THREADED,
			m_D2DFactory.ReleaseAndGetAddressOf()
		);
		if (FAILED(hr))
			throw Core::Error::COMError(__FUNCSIG__": D2D1CreateFactory() failed", hr);
	}

	Microsoft::WRL::ComPtr<ID2D1Factory> D2DFactory::Get() const noexcept
	{
		return m_D2DFactory;
	}
	
	ID2D1Factory* D2DFactory::GetRaw() const noexcept
	{
		return m_D2DFactory.Get();
	}

	D2D1_FACTORY_TYPE D2DFactory::GetFactoryType() const noexcept
	{
		return m_factoryType;
	}
}