export module spacedefender:assetmanager;
import std;

export namespace SpaceDefender
{
	struct BitmapAsset
	{
		std::filesystem::path Path;

	};

	class AssetManager
	{
	public:
		AssetManager() = default;
		AssetManager(const AssetManager&) = delete;
		auto operator=(const AssetManager&) -> AssetManager& = delete;
		AssetManager(AssetManager&&) noexcept = default;
		auto operator=(AssetManager&&) -> AssetManager& = default;
	private:
	};
}
