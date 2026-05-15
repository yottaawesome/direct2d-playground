export module shared:wic.formatconverter;
import std;
import :win32;
import :error;
import :com;
import :wic.bitmapframedecode;

export namespace Shared
{
	struct ConverterParameters
	{
		WIC::IWICBitmapFrameDecode* Source = nullptr;
		Win32::GUID DestinationFormat = {};
		WIC::WICBitmapDitherType DitherType = WIC::WICBitmapDitherType::WICBitmapDitherTypeNone;
		WIC::IWICPalette* Palette = nullptr;
		double AlphaThresholdPercent = 0.0;
		WIC::WICBitmapPaletteType PaletteTranslationType = WIC::WICBitmapPaletteType::WICBitmapPaletteTypeCustom;
	};

	class FormatConverter
	{
	public:
		FormatConverter(const FormatConverter&) = delete;
		auto operator=(const FormatConverter&) -> FormatConverter& = delete;

		FormatConverter(FormatConverter&&) noexcept = default;
		auto operator=(FormatConverter&&) -> FormatConverter & = default;

		FormatConverter(Ptr<WIC::IWICFormatConverter> converter)
			: converter(std::move(converter))
		{ }

		auto operator->(this auto&& self) noexcept -> WIC::IWICFormatConverter*
		{
			return self.converter.Get();
		}

	protected:
		Ptr<WIC::IWICFormatConverter> converter;
	};
}
