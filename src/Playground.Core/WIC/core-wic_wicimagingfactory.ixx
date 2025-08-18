module;

#include <Windows.h>
#include <wincodec.h>
#include <wrl/client.h>

export module core:wic_wicimagingfactory;
import std;
import :error_comerror;

export namespace Core::WIC
{
	class WICImagingFactory
	{
	public:
		virtual ~WICImagingFactory()
		{
			Close();
		}

		WICImagingFactory()
		{
			// https://docs.microsoft.com/en-us/windows/win32/api/wincodec/nn-wincodec-iwicimagingfactory
			HRESULT hr = CoCreateInstance(
				CLSID_WICImagingFactory,
				nullptr,
				CLSCTX_INPROC_SERVER,
				IID_PPV_ARGS(&m_imagingFactory)
			);
			if (FAILED(hr))
				throw Error::COMError(__FUNCSIG__": CoCreateInstance() failed", hr);
		}

	public:
		virtual void Close()
		{
			m_imagingFactory = nullptr;
		}

		virtual Microsoft::WRL::ComPtr<IWICBitmapDecoder> CreateDecoderFromFilename(const std::wstring& path)
		{
			if (!m_imagingFactory)
				throw std::runtime_error(__FUNCSIG__": m_imagingFactory is nullptr");

			Microsoft::WRL::ComPtr<IWICBitmapDecoder> result;
			// https://docs.microsoft.com/en-us/windows/win32/api/wincodec/nf-wincodec-iwicimagingfactory-createdecoderfromfilename
			const HRESULT hr = m_imagingFactory->CreateDecoderFromFilename(
				path.c_str(),
				nullptr,
				GENERIC_READ,
				WICDecodeMetadataCacheOnLoad,
				&result
			);
			if (FAILED(hr))
				throw Error::COMError(__FUNCSIG__": CreateDecoderFromFilename() failed", hr);

			return result;
		}

		virtual Microsoft::WRL::ComPtr<IWICFormatConverter> CreateFormatConverter()
		{
			if (!m_imagingFactory)
				throw std::runtime_error(__FUNCSIG__": m_imagingFactory is nullptr");

			Microsoft::WRL::ComPtr<IWICFormatConverter> pConverter;
			HRESULT hr = m_imagingFactory->CreateFormatConverter(&pConverter);
			if (FAILED(hr))
				throw Core::Error::COMError("CreateFormatConverter() failed", hr);

			return pConverter;
		}

	protected:
		Microsoft::WRL::ComPtr<IWICImagingFactory> m_imagingFactory;
	};
}