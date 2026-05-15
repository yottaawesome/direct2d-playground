export module spacedefender:assetmanager;
import std;
import shared;
import :wiccomponents;

export namespace SpaceDefender
{
	class AssetManager
	{
	public:
		AssetManager() = default;
		AssetManager(const AssetManager&) = delete;
		auto operator=(const AssetManager&) -> AssetManager& = delete;
		AssetManager(AssetManager&&) noexcept = default;
		auto operator=(AssetManager&&) -> AssetManager& = default;

		Shared::Bitmap Player;
		Shared::Bitmap Enemy;
		Shared::Bitmap PlayerBullet;
		Shared::Bitmap EnemyBullet;

		auto Load(this auto&& self, D2D1::ID2D1DeviceContext* deviceContext)
		{
			if (self.Player)
				return;

			auto baseDirectory = std::filesystem::path{ "assets" };
			auto playerSprite = baseDirectory / "player.png";
			auto enemySprite = baseDirectory / "enemy.png";
			auto playerBulletSprite = baseDirectory / "player-bullet.png";
			auto enemyBulletSprite = baseDirectory / "enemy-bullet.png";

			self.Player = self.CreateBitmapFromFile(playerSprite, deviceContext);
			self.Enemy = self.CreateBitmapFromFile(enemySprite, deviceContext);
			self.PlayerBullet = self.CreateBitmapFromFile(playerBulletSprite, deviceContext);
			self.EnemyBullet = self.CreateBitmapFromFile(enemyBulletSprite, deviceContext);
		}

		auto Discard()
		{
			EnemyBullet = {};
			PlayerBullet = {};
			Enemy = {};
			Player = {};
		}

		[[nodiscard]]
		auto CreateBitmapFromFile(
			this auto&& self,
			const std::filesystem::path& filename,
			D2D1::ID2D1DeviceContext* deviceContext
		) -> Shared::Bitmap
		{
			if (not deviceContext)
				throw Shared::Error{ "Device context cannot be null when creating bitmap from file" };
			if (not std::filesystem::exists(filename))
				throw Shared::Error{ std::format("File not found: {}", filename.string()) };

			auto bitmapDecoder = Shared::BitmapDecoder{ self.wicContext.CreateDecoderFromFilename({.Filename = filename }) };
			auto frame = Shared::BitmapFrameDecode{ bitmapDecoder.GetFrame(0) };
			auto converter = Shared::FormatConverter{
				self.wicContext.CreateConverter({
					.Source = frame.Get(),
					.DestinationFormat = WIC::GUID_WICPixelFormat32bppPBGRA
				}) };

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
		Shared::WicContext wicContext{};
	};
}
