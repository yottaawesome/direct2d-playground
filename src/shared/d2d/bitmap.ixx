export module shared:d2d.bitmap;
import :win32;
import :error;
import :com;

export namespace Shared
{
	class Bitmap
	{
	public:
		struct View
		{
			View() = default;

			constexpr View(D2D1::ID2D1Bitmap1* bitmap)
				: bitmap(bitmap)
			{}

			auto operator->(this auto&& self) noexcept -> D2D1::ID2D1Bitmap1*
			{
				return self.bitmap;
			}

			auto Get(this auto&& self) noexcept -> D2D1::ID2D1Bitmap1*
			{
				return self.bitmap;
			}

			explicit operator bool() const noexcept
			{
				return bitmap != nullptr;
			}

			auto GetCenter(this auto&& self) -> D2D1::D2D1_POINT_2F
			{
				auto [width, height] = self.GetSize();
				return { width / 2.0f, height / 2.0f };
			}

			auto GetSize(this auto&& self) -> std::pair<float, float>
			{
				auto size = D2D1::D2D1_SIZE_F{ self.bitmap->GetSize() };
				return { size.width, size.height };
			}

			D2D1::ID2D1Bitmap1* bitmap = nullptr;
		};

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

		auto GetCenter(this auto&& self) -> D2D1::D2D1_POINT_2F
		{
			auto [width, height] = self.GetSize();
			return { width / 2.0f, height / 2.0f };
		}

		auto GetSize(this auto&& self) -> std::pair<float, float>
		{
			auto size = D2D1::D2D1_SIZE_F{self.bitmap->GetSize()};
			return { size.width, size.height };
		}

		auto ToView(this auto&& self) -> View
		{
			return View{ self.bitmap.Get() };
		}
	protected:
		Ptr<D2D1::ID2D1Bitmap1> bitmap;
	};
}