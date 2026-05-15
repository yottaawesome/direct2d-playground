export module shared:d2d.bitmap;
import :win32;
import :error;
import :com;

export namespace Shared
{
	class Bitmap
	{
	public:
		Bitmap() = default;
		Bitmap(const Bitmap&) noexcept = delete;
		auto operator=(const Bitmap&) noexcept -> Bitmap& = delete;
		Bitmap(Bitmap&&) noexcept = default;
		auto operator=(Bitmap&&) noexcept -> Bitmap& = default;
		
		constexpr Bitmap(Ptr<D2D1::ID2D1Bitmap1> bitmap)
			: bitmap(std::move(bitmap))
		{ }
		auto operator->(this auto&& self) noexcept -> D2D1::ID2D1Bitmap1*
		{
			return self.bitmap.Get();
		}

		auto Get(this auto&& self) noexcept -> D2D1::ID2D1Bitmap1*
		{
			return self.bitmap.Get();
		}

		explicit operator bool() const noexcept
		{
			return static_cast<bool>(bitmap);
		}
	protected:
		Ptr<D2D1::ID2D1Bitmap1> bitmap;
	};
}