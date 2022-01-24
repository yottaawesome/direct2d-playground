module;

#include <stdexcept>
#include <windows.h>
#include <d2d1.h>
#include <d2d1helper.h>
#include <dwrite.h>
#include <wrl/client.h>

module spaceinvaders.rendering.renderer;
import core.error;

namespace SpaceInvaders::Rendering
{
	Renderer::~Renderer()
	{
		Close();
	}

	Renderer::Renderer() : m_factoryType(D2D1_FACTORY_TYPE::D2D1_FACTORY_TYPE_SINGLE_THREADED) {}

	Renderer::Renderer(const D2D1_FACTORY_TYPE factoryType)
	{
		Initialise(factoryType);
	}

	Renderer::operator bool() const noexcept
	{
		return m_pDirect2dFactory != nullptr;
	}

	Renderer::operator ID2D1Factory* () const noexcept
	{
		return m_pDirect2dFactory.Get();
	}

	void Renderer::Close()
	{
		m_pDirect2dFactory = nullptr;
	}

	Microsoft::WRL::ComPtr<ID2D1HwndRenderTarget> Renderer::CreateHwndRenderTarget(
		const HWND hwnd,
		const UINT32 width,
		const UINT32 height
	)
	{
		if (!hwnd)
			throw std::invalid_argument(__FUNCSIG__": hwnd is null");
		if (!m_pDirect2dFactory)
			Initialise(m_factoryType);

		const D2D1_SIZE_U size = D2D1::SizeU(width, height);
		// Create a Direct2D render target.
		Microsoft::WRL::ComPtr<ID2D1HwndRenderTarget> renderTarget;
		// https://docs.microsoft.com/en-us/windows/win32/api/d2d1/nf-d2d1-id2d1factory-createhwndrendertarget(constd2d1_render_target_properties__constd2d1_hwnd_render_target_properties__id2d1hwndrendertarget)
		// https://docs.microsoft.com/en-us/windows/win32/api/d2d1/nf-d2d1-id2d1factory-createhwndrendertarget(constd2d1_render_target_properties_constd2d1_hwnd_render_target_properties_id2d1hwndrendertarget)
		const HRESULT hr = m_pDirect2dFactory->CreateHwndRenderTarget(
			D2D1::RenderTargetProperties(),
			D2D1::HwndRenderTargetProperties(hwnd, size),
			&renderTarget
		);
		if (FAILED(hr))
			throw Core::Error::COMError(__FUNCSIG__": CreateHwndRenderTarget() failed", hr);

		return renderTarget;
	}

	void Renderer::Initialise(const D2D1_FACTORY_TYPE factoryType)
	{
		if (m_pDirect2dFactory)
			throw std::runtime_error(__FUNCSIG__": already initialised");

		// Create a Direct2D factory.
		// https://docs.microsoft.com/en-us/windows/win32/api/d2d1/nf-d2d1-d2d1createfactory
		m_factoryType = factoryType;
		const HRESULT hr = D2D1CreateFactory(
			factoryType, //D2D1_FACTORY_TYPE_SINGLE_THREADED,
			m_pDirect2dFactory.ReleaseAndGetAddressOf()
		);
		if (FAILED(hr))
			throw Core::Error::COMError(__FUNCSIG__": D2D1CreateFactory() failed", hr);
	}

	D2D1_FACTORY_TYPE Renderer::GetFactoryType() const noexcept
	{
		return m_factoryType;
	}

	Microsoft::WRL::ComPtr<ID2D1Factory> Renderer::Get() const noexcept
	{
		return m_pDirect2dFactory;
	}

	ID2D1Factory* Renderer::GetRaw() const noexcept
	{
		return m_pDirect2dFactory.Get();
	}
}