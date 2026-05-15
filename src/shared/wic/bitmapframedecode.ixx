export module shared:wic.bitmapframedecode;
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
		auto operator=(const BitmapFrameDecode&) -> BitmapFrameDecode & = delete;
		BitmapFrameDecode(BitmapFrameDecode&&) noexcept = default;
		auto operator=(BitmapFrameDecode&&) -> BitmapFrameDecode & = default;

		auto operator->(this auto&& self) noexcept -> WIC::IWICBitmapFrameDecode*
		{
			return self.frame.Get();
		}

		auto GetSize(this auto&& self) -> std::pair<std::uint32_t, std::uint32_t>
		{
			auto width = std::uint32_t{};
			auto height = std::uint32_t{};
			auto hr = Shared::HResult{ self.frame->GetSize(&width, &height) };
			if (not hr)
				throw Shared::ComError{ hr, "GetSize() failed for bitmap frame" };
			return { width, height };
		}

		auto Get(this auto&& self) noexcept -> WIC::IWICBitmapFrameDecode*
		{
			return self.frame.Get();
		}

	protected:
		Ptr<WIC::IWICBitmapFrameDecode> frame;
	};
}
