export module spacedefender:entity;
import std;
import shared;
import :assetmanager;

export namespace SpaceDefender
{
	struct Background
	{
		D2D1::ID2D1Bitmap* Sprite = nullptr;
	};

	enum class EntityType
	{
		Player,
		Enemy,
		PlayerBullet,
		EnemyBullet,
	};

	constexpr auto MaximumSpriteCount = 50;
	using Degrees = float;
	struct EntityDetails
	{
		EntityType Type;
		bool Active;
		SpriteType Sprite;
		Shared::Vector2 Position;
		Shared::Vector2 Velocity;
		Degrees Rotation;
	};

	enum class EnemyType
	{
		Common,
	};

	

	struct Enemies
	{
		std::vector<EnemyType> Types{ MaximumSpriteCount };
		std::vector<std::uint32_t> Active = Shared::MakeVector<std::uint32_t, MaximumSpriteCount>();
		std::vector<SpriteType> SpriteCollection{ MaximumSpriteCount };
		std::vector<Shared::Vector2> Positions{ MaximumSpriteCount };
		std::vector<Shared::Vector2> Velocities{ MaximumSpriteCount };
		std::vector<Degrees> Rotations = Shared::MakeVector<Degrees, MaximumSpriteCount>();
		auto Size() -> size_t
		{
			return Types.size();
		}
	};

	struct Player
	{
		SpriteType Sprite;
		Shared::Vector2 Position;
		Shared::Vector2 Velocity;
		Degrees Rotation;
	};

	struct EnemyDetails
	{
		EnemyType Type =  EnemyType::Common;
		bool Active = true;
		SpriteType Sprite = SpriteType::Enemy;
		Shared::Vector2 Position = {};
		Shared::Vector2 Velocity = {};
		Degrees Rotation = 0;
	};

	struct PlayerBullets
	{
		std::vector<std::uint8_t> Active = Shared::MakeVector<std::uint8_t, MaximumSpriteCount>();
		std::vector<SpriteType> SpriteCollection{ MaximumSpriteCount };
		std::vector<Shared::Vector2> Positions{ MaximumSpriteCount };
		std::vector<Shared::Vector2> Velocities{ MaximumSpriteCount };
		std::vector<Degrees> Rotations = 
			Shared::MakeVector<Degrees, MaximumSpriteCount>();
		std::vector<float> TimeAlive = Shared::MakeVector<float, MaximumSpriteCount>();
	};
	struct PlayerBulletDetails
	{
		bool Active = false;
		SpriteType Sprite = SpriteType::PlayerBullet;
		Shared::Vector2 Position = {};
		Shared::Vector2 Velocity = {};
		Degrees Rotation = 0;
	};

	struct EntityCollection
	{
		Player Player{};
		Enemies Enemies{};
		PlayerBullets PlayerBullets{};

		auto AddBullet(this auto&& self, PlayerBulletDetails bullet) -> std::size_t
		{
			for (auto i = 0ull; i < self.PlayerBullets.Active.size(); ++i)
			{
				if (self.PlayerBullets.Active[i])
					continue;
				self.PlayerBullets.Active[i] = bullet.Active;
				self.PlayerBullets.SpriteCollection[i] = bullet.Sprite;
				self.PlayerBullets.Positions[i] = bullet.Position;
				self.PlayerBullets.Velocities[i] = bullet.Velocity;
				self.PlayerBullets.Rotations[i] = bullet.Rotation;
				self.PlayerBullets.TimeAlive[i] = 0.0f;
				return i;
			}
			throw std::runtime_error{ "Maximum player bullet count reached" };
		}

		auto AddEnemy(this auto&& self, EnemyDetails enemy) -> std::size_t
		{
			for (auto i = 0ull; i < self.Enemies.Types.size(); ++i)
			{
				if (self.Enemies.Active[i])
					continue;
				self.Enemies.Types[i] = enemy.Type;
				self.Enemies.Active[i] = enemy.Active;
				self.Enemies.SpriteCollection[i] = enemy.Sprite;
				self.Enemies.Positions[i] = enemy.Position;
				self.Enemies.Velocities[i] = enemy.Velocity;
				self.Enemies.Rotations[i] = enemy.Rotation;
				return i;
			}
			throw std::runtime_error{ "Maximum enemy count reached" };
		}
	};
}
