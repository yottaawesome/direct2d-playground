export module shared:wic.bitmapdecoder;
import std;
import :error;
import :win32;
import :com;
import :wic.bitmapframedecode;

export namespace Shared
{
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
				throw Shared::ComError{ hr, std::format("GetFrame() failed for frame index {}", index) };
			return BitmapFrameDecode{ std::move(frame) };
		}
	protected:
		Ptr<WIC::IWICBitmapDecoder> decoder;
	};
}
