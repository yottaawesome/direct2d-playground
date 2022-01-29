module;

#include <stdexcept>
#include <source_location>
#include <windows.h>
#include <d2d1.h>
#include <d2d1helper.h>
#include <dwrite.h>
#include <wincodec.h>
#include <wrl/client.h>

module spaceinvaders.engine.renderer;
import core.error;
import core.wic.wicimagingfactory;

namespace SpaceInvaders::Engine
{
	Renderer::~Renderer()
	{
		Close();
	}
	
	Renderer::Renderer() {}

	Renderer::Renderer(
		const Microsoft::WRL::ComPtr<ID2D1HwndRenderTarget>& hwndRenderTarget
	)
		: m_hwndRenderTarget(hwndRenderTarget)
	{ }

	Renderer::Renderer(Renderer&& other) noexcept = default;
	Renderer& Renderer::operator=(Renderer&&) noexcept = default;

	Renderer::operator bool() const noexcept
	{
		return m_hwndRenderTarget != nullptr;
	}

	void Renderer::Close()
	{
		m_hwndRenderTarget = nullptr;
	}

	void Renderer::Clear(const D2D1::ColorF& color)
	{
		m_hwndRenderTarget->Clear(color);
	}

	void Renderer::Draw(const DrawFunction& DrawToTarget)
	{
		m_hwndRenderTarget->BeginDraw();
		DrawToTarget();
		if (const HRESULT hr = m_hwndRenderTarget->EndDraw(); FAILED(hr))
			throw Core::Error::COMError(__FUNCSIG__": EndDraw() failed", hr);
	}
	
	void Renderer::Resize(const unsigned width, const unsigned height)
	{
		if (m_hwndRenderTarget)
			m_hwndRenderTarget->Resize(D2D1::SizeU(width, height));
	}

	ID2D1HwndRenderTarget* Renderer::GetRaw() const noexcept
	{
		return m_hwndRenderTarget.Get();
	}

	Microsoft::WRL::ComPtr<ID2D1Bitmap> Renderer::LoadBitmap(const std::wstring& path)
	{
		Microsoft::WRL::ComPtr<ID2D1Bitmap> m_bitmap;

		//// https://docs.microsoft.com/en-us/windows/win32/Direct2D/how-to-load-a-direct2d-bitmap-from-a-file
		Core::WIC::WICImagingFactory factory;
		Microsoft::WRL::ComPtr<IWICBitmapDecoder> pDecoder = factory.CreateDecoderFromFilename(path);

		Microsoft::WRL::ComPtr<IWICBitmapFrameDecode> pSource;
		//// Create the initial frame.
		HRESULT hr = pDecoder->GetFrame(0, &pSource);
		if (FAILED(hr))
			throw Core::Error::COMError("GetFrame() failed", hr);

		Microsoft::WRL::ComPtr<IWICFormatConverter> pConverter = factory.CreateFormatConverter();
		hr = pConverter->Initialize(
			pSource.Get(),
			GUID_WICPixelFormat32bppPBGRA,
			WICBitmapDitherTypeNone,
			nullptr,
			0.f,
			WICBitmapPaletteTypeMedianCut
		);
		if (FAILED(hr))
			throw Core::Error::COMError("Initialize() failed", hr);

		hr = m_hwndRenderTarget->CreateBitmapFromWicBitmap(
			pConverter.Get(),
			nullptr,
			&m_bitmap
		);
		if (FAILED(hr))
			throw Core::Error::COMError("CreateBitmapFromWicBitmap() failed", hr);

		return m_bitmap;
	}
}