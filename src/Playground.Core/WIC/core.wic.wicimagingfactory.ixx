module;

#include <string>
#include <Windows.h>
#include <wincodec.h>
#include <wrl/client.h>

export module core.wic.wicimagingfactory;

export namespace Core::WIC
{
	class WICImagingFactory
	{
		public:
			virtual ~WICImagingFactory();
			WICImagingFactory();

		public:
			virtual void Close();
			virtual Microsoft::WRL::ComPtr<IWICBitmapDecoder> CreateDecoderFromFilename(const std::wstring& path);
			virtual Microsoft::WRL::ComPtr<IWICFormatConverter> CreateFormatConverter();

		protected:
			Microsoft::WRL::ComPtr<IWICImagingFactory> m_imagingFactory;
	};
}