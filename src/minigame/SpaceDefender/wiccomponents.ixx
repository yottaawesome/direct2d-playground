export module spacedefender:wiccomponents;
import std;
import shared;

export namespace SpaceDefender
{
	class WicComponents
	{
	public:
		auto CreateBitmapFromFile(
			this auto&& self,
			const std::filesystem::path& filename,
			D2D1::ID2D1DeviceContext* deviceContext
		) -> Shared::Ptr<D2D1::ID2D1Bitmap1>
		{
			if (not deviceContext)
				throw Shared::Error{ "Device context cannot be null when creating bitmap from file" };
			if (not std::filesystem::exists(filename))
				throw Shared::Error{ std::format("File not found: {}", filename.string()) };

			auto playerDecoder = self.wicContext.CreateDecoderFromFilename({ .Filename = filename });
			auto frame = playerDecoder.GetFrame(0);

			auto converter = self.wicContext.CreateConverter({
				.Source = frame.Get(),
				.DestinationFormat = WIC::GUID_WICPixelFormat32bppPBGRA
			});

			auto bitmap = Shared::Ptr<ID2D1Bitmap1>{};
			auto hr = Shared::HResult{
				deviceContext->CreateBitmapFromWicBitmap(
					converter.Get(),
					nullptr,
					bitmap.ReleaseAndGetAddressOf()
				) };
			if (not hr)
				throw Shared::ComError{ hr, "Failed to create D2D bitmap from WIC bitmap" };

			return bitmap;
		}
	private:
		Shared::WicContext wicContext{};
	};
}
