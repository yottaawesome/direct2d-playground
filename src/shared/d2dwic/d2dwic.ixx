export module shared:d2dwic;
import std;
import :win32;
import :wic;
import :d2d;

export namespace Shared
{
	class WicD2DContext
	{
	public:
		WicD2DContext(const WicD2DContext&) = delete;
		auto operator=(const WicD2DContext&) -> WicD2DContext& = delete;
		WicD2DContext(WicD2DContext&&) noexcept = default;
		auto operator=(WicD2DContext&&) -> WicD2DContext& = default;
		
		WicD2DContext(DeviceContext& deviceContext, WicContext& wicContext)
			: deviceContext(deviceContext), wicContext(wicContext)
		{}

		[[nodiscard]]
		auto CreateBitmapFromFile(this auto&& self, const std::filesystem::path& filename) -> Bitmap
		{
			if (not std::filesystem::exists(filename))
				throw Shared::Error{ std::format("File not found: {}", filename.string()) };

			auto bitmapDecoder = Shared::BitmapDecoder{ self.wicContext.CreateDecoderFromFilename({.Filename = filename }) };
			auto frame = Shared::BitmapFrameDecode{ bitmapDecoder.GetFrame(0) };
			auto converter = Shared::FormatConverter{
				self.wicContext.CreateConverter({
					.Source = frame.Get(),
					.DestinationFormat = WIC::GUID_WICPixelFormat32bppPBGRA
				})};

			auto bitmap = Shared::Ptr<ID2D1Bitmap1>{};
			auto hr = Shared::HResult{
				deviceContext->CreateBitmapFromWicBitmap(
					converter.Get(),
					nullptr,
					bitmap.ReleaseAndGetAddressOf()
				) };
			if (not hr)
				throw Shared::ComError{ hr, "Failed to create D2D bitmap from WIC bitmap" };

			return { bitmap };
		}

	private:
		DeviceContext& deviceContext;
		WicContext& wicContext;
	};
}