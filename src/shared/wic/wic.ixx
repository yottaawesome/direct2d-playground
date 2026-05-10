export module shared:wic;
import std;
import :error;
import :win32;
import :com;

export namespace Shared
{
	class BitmapFrameDecode
	{
	public:
		BitmapFrameDecode(Ptr<WIC::IWICBitmapFrameDecode> frame)
			: frame(std::move(frame)) {}
		BitmapFrameDecode(const BitmapFrameDecode&) = delete;
		auto operator=(const BitmapFrameDecode&) -> BitmapFrameDecode& = delete;
		BitmapFrameDecode(BitmapFrameDecode&&) noexcept = default;
		auto operator=(BitmapFrameDecode&&) -> BitmapFrameDecode& = default;

		auto operator->(this auto&& self) noexcept -> WIC::IWICBitmapFrameDecode*
		{
			return self.frame.Get();
		}

	protected:
		Ptr<WIC::IWICBitmapFrameDecode> frame;
	};

	class BitmapDecoder
	{
	public:
		BitmapDecoder(const BitmapDecoder&) = delete;
		auto operator=(const BitmapDecoder&) -> BitmapDecoder& = delete;
		BitmapDecoder(BitmapDecoder&&) noexcept = default;
		auto operator=(BitmapDecoder&&) -> BitmapDecoder& = default;
		BitmapDecoder(Ptr<WIC::IWICBitmapDecoder> decoder)
			: decoder(std::move(decoder)) {}

		auto operator->(this auto&& self) noexcept -> WIC::IWICBitmapDecoder*
		{
			return self.decoder.Get();
		}

		auto GetFrame(this auto&& self, std::uint32_t index) -> BitmapFrameDecode
		{
			auto frame = Ptr<WIC::IWICBitmapFrameDecode>{};
			auto hr = Shared::HResult{ self.decoder->GetFrame(index, frame.AddressOfTyped()) };
			if (not hr)
				throw Shared::ComError{ hr, std::format( "GetFrame() failed for frame index {}", index ) };
			return BitmapFrameDecode{ std::move(frame) };
		}
	protected:
		Ptr<WIC::IWICBitmapDecoder> decoder;
	};

	class WicContext
	{
	public:
		WicContext() = default;

		WicContext(const WicContext&) = delete;
		auto operator=(const WicContext&) -> WicContext& = delete;
		WicContext(WicContext&&) noexcept = default;
		auto operator=(WicContext&&) -> WicContext& = default;

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
				throw Shared::Error{ std::format( "File not found: {}", options.Filename.string() ) };

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
				throw Shared::ComError{ hr, std::format( "CreateDecoderFromFilename() failed for {}", options.Filename.string() ) };
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