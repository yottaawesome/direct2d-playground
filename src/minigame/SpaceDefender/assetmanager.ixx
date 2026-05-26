export module spacedefender:assetmanager;
import std;
import shared;
import :wiccomponents;

export namespace SpaceDefender
{
	enum class SpriteType : size_t
	{
		Player,
		Enemy,
		PlayerBullet,
		EnemyBullet,
	};

	struct AllBitmaps
	{
		std::array<Shared::Bitmap, 4> Bitmaps{};
		auto operator[](this auto&& self, SpriteType type) -> Shared::Bitmap&
		{
			return self.Bitmaps[static_cast<std::size_t>(type)];
		}
		auto Clear(this auto&& self)
		{
			for (auto& bitmap : self.Bitmaps)
				bitmap = {};
		}
	};

	struct AllBrushes
	{
		Shared::Ptr<D2D1::ID2D1SolidColorBrush> DebugBrush;
		auto Clear(this auto&& self)
		{
			self.DebugBrush = nullptr;
		}
	};

	class AssetManager
	{
	public:
		AssetManager() = default;
		AssetManager(const AssetManager&) = delete;
		auto operator=(const AssetManager&) -> AssetManager& = delete;
		AssetManager(AssetManager&&) noexcept = default;
		auto operator=(AssetManager&&) -> AssetManager& = default;

		AllBitmaps Bitmaps{};
		AllBrushes SolidColorBrushes{};
		auto operator[](this auto&& self, SpriteType type) -> Shared::Bitmap&
		{
			return self.Bitmaps[type];
		}

		auto Load(this auto&& self, D2D1::ID2D1DeviceContext* deviceContext)
		{
			self.LoadSprites(deviceContext);
			self.LoadBrushes(deviceContext);
		}

		auto Discard(this auto&& self)
		{
			self.Bitmaps.Clear();
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
				throw Shared::Error{ std::format("File not found: {}", filename.string()), Shared::ErrorExitCode::AssetsNotFound };

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
		auto LoadSprites(this auto&& self, D2D1::ID2D1DeviceContext* deviceContext)
		{
			if (self[SpriteType::Player])
				return;

			auto baseDirectory = std::filesystem::path{ "assets" };
			auto playerSprite = baseDirectory / "player.png";
			auto enemySprite = baseDirectory / "enemy.png";
			auto playerBulletSprite = baseDirectory / "player-bullet.png";
			auto enemyBulletSprite = baseDirectory / "enemy-bullet.png";

			self[SpriteType::Player] = self.CreateBitmapFromFile(playerSprite, deviceContext);
			self[SpriteType::Enemy] = self.CreateBitmapFromFile(enemySprite, deviceContext);
			self[SpriteType::PlayerBullet] = self.CreateBitmapFromFile(playerBulletSprite, deviceContext);
			self[SpriteType::EnemyBullet] = self.CreateBitmapFromFile(enemyBulletSprite, deviceContext);
		}

		auto LoadBrushes(this auto&& self, D2D1::ID2D1DeviceContext* deviceContext)
		{
			auto hr = Shared::HResult{
				deviceContext->CreateSolidColorBrush(
					D2D1::ColorF(D2D1::ColorF::DarkRed),
					self.SolidColorBrushes.DebugBrush.ReleaseAndGetAddressOf()
				) };
			if (not hr)
				throw Shared::ComError{ hr, "Failed to create solid color brush" };
		}

		Shared::WicContext wicContext{};
	};
}
