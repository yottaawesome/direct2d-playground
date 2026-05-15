export module shared:wic.wiccontext;
import std;
import :error;
import :win32;
import :com;
import :wic.bitmapdecoder;
import :wic.formatconverter;

export namespace Shared
{
	class WicContext
	{
	public:
		WicContext() = default;

		WicContext(const WicContext&) = delete;
		auto operator=(const WicContext&) -> WicContext & = delete;
		WicContext(WicContext&&) noexcept = default;
		auto operator=(WicContext&&) -> WicContext & = default;

		auto operator->(this auto&& self) noexcept -> WIC::IWICImagingFactory2*
		{
			return self.Factory.Get();
		}

		auto operator*(this auto&& self) noexcept -> WIC::IWICImagingFactory2&
		{
			return *self.Factory.Get();
		}

		auto Get(this auto&& self) noexcept -> WIC::IWICImagingFactory2*
		{
			return self.Factory.Get();
		}

		auto CreateConverter(
			this auto&& self, 
			const ConverterParameters& params
		) -> FormatConverter
		{
			if (not params.Source)
				throw Error{ "Source bitmap frame decode cannot be null" };

			auto converter = Ptr<WIC::IWICFormatConverter>{};
			auto hr = HResult{ self.Factory->CreateFormatConverter(converter.AddressOfTyped()) };
			if (not hr)
				throw ComError{ hr, "CreateFormatConverter() failed" };

			hr = 
				converter->Initialize(
					params.Source,
					params.DestinationFormat,
					params.DitherType,
					params.Palette,
					params.AlphaThresholdPercent,
					params.PaletteTranslationType
				);
			if (not hr)
				throw Shared::ComError{ hr, "Failed to initialize format converter for destination format" };

			return FormatConverter{ std::move(converter) };
		}

		template<typename T>
		auto As(this auto&& self) -> Ptr<T>
		{
			return self.Factory.As<T>();
		}

		struct CreateDecoderFromFilenameOptions
		{
			std::filesystem::path Filename;
			Win32::DWORD Access = Win32::Access::Read;
			std::optional<Win32::GUID> Vendor = std::nullopt;
			WIC::WICDecodeOptions DecodeOptions = WIC::WICDecodeOptions::WICDecodeMetadataCacheOnDemand;
		};

		auto CreateDecoderFromFilename(
			this auto&& self,
			const CreateDecoderFromFilenameOptions& options
		) -> BitmapDecoder
		{
			if (not self.Factory)
				throw Shared::Error{ "WIC Imaging Factory is not initialized." };
			if (not std::filesystem::exists(options.Filename))
				throw Shared::Error{ std::format("File not found: {}", options.Filename.string()) };

			auto decoder = Shared::Ptr<WIC::IWICBitmapDecoder>{};
			auto hr = Shared::HResult{
				self.Factory->CreateDecoderFromFilename(
					options.Filename.c_str(),
					options.Vendor ? &*options.Vendor : nullptr,
					options.Access,
					options.DecodeOptions,
					decoder.AddressOfTyped()
				) };
			if (not hr)
				throw Shared::ComError{ hr, std::format("CreateDecoderFromFilename() failed for {}", options.Filename.string()) };
			return BitmapDecoder{ std::move(decoder) };
		}

	private:
		Ptr<WIC::IWICImagingFactory2> Factory =
			[] -> Ptr<WIC::IWICImagingFactory2>
			{
				auto factory = Ptr<WIC::IWICImagingFactory2>{};
				auto hr = HResult{
					Win32::CoCreateInstance(
						WIC::CLSID_WICImagingFactory2,
						nullptr,
						Win32::CLSCTX::CLSCTX_INPROC_SERVER,
						factory.GetUuid(),
						factory.AddressOf()
					) };
				if (not hr)
					throw ComError{ hr, "CoCreateInstance(CLSID_WICImagingFactory2) failed" };
				return factory;
			}();
	};
}