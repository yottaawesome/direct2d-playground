export module core:wic_wicimagingfactory;
import std;
import :win32;
import :error_comerror;

export namespace WIC
{
	class WICImagingFactory final
	{
	public:
		~WICImagingFactory()
		{
			Close();
		}

		WICImagingFactory()
		{
			// https://docs.microsoft.com/en-us/windows/win32/api/wincodec/nn-wincodec-iwicimagingfactory
			Win32::HRESULT hr = Win32::CoCreateInstance(
				Win32::CLSID_WICImagingFactory2,
				nullptr,
				Win32::CLSCTX::CLSCTX_INPROC_SERVER,
				Win32::IID_IWICImagingFactory2,
				&m_imagingFactory
			);
			if (Win32::HrFailed(hr))
				throw Error::COMError("CoCreateInstance() failed", hr);
		}

		void Close()
		{
			m_imagingFactory = nullptr;
		}

		auto CreateDecoderFromFilename(const std::wstring& path) -> Win32::ComPtr<Win32::IWICBitmapDecoder>
		{
			if (!m_imagingFactory)
				throw std::runtime_error("m_imagingFactory is nullptr");

			Microsoft::WRL::ComPtr<Win32::IWICBitmapDecoder> result;
			// https://docs.microsoft.com/en-us/windows/win32/api/wincodec/nf-wincodec-iwicimagingfactory-createdecoderfromfilename
			Win32::HRESULT hr = m_imagingFactory->CreateDecoderFromFilename(
				path.c_str(),
				nullptr,
				Win32::GenericRead,
				WICDecodeMetadataCacheOnLoad,
				&result
			);
			if (Win32::HrFailed(hr))
				throw Error::COMError("CreateDecoderFromFilename() failed", hr);

			return result;
		}

		auto CreateFormatConverter() -> Win32::ComPtr<Win32::IWICFormatConverter>
		{
			if (!m_imagingFactory)
				throw std::runtime_error("m_imagingFactory is nullptr");

			Win32::ComPtr<Win32::IWICFormatConverter> pConverter;
			Win32::HRESULT hr = m_imagingFactory->CreateFormatConverter(&pConverter);
			if (Win32::HrFailed(hr))
				throw Error::COMError("CreateFormatConverter() failed", hr);

			return pConverter;
		}

	private:
		Win32::ComPtr<Win32::IWICImagingFactory2> m_imagingFactory;
	};
}