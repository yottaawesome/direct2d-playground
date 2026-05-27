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
	struct Vector2
	{
		float X = 0.0f;
		float Y = 0.0f;
	};

	enum class EntityType
	{
		Player,
		Enemy,
		PlayerBullet,
		EnemyBullet,
	};

	constexpr auto MaximumSpriteCount = 25;
	using Degrees = float;
	struct EntityDetails
	{
		EntityType Type;
		bool Active;
		SpriteType Sprite;
		Vector2 Position;
		Vector2 Velocity;
		Degrees Rotation;
	};

	enum class EnemyType
	{
		Common,
	};
	struct Enemies
	{
		std::vector<EnemyType> Types{ MaximumSpriteCount };
		std::vector<std::uint32_t> Active = 
			[] static->std::vector<std::uint32_t>
			{
				auto active = std::vector<std::uint32_t>{};
				active.resize(MaximumSpriteCount);
				return active;
			}();
		std::vector<SpriteType> SpriteCollection{ MaximumSpriteCount };
		std::vector<Vector2> Positions{ MaximumSpriteCount };
		std::vector<Vector2> Velocities{ MaximumSpriteCount };
		std::vector<Degrees> Rotations =
			[] static->std::vector<Degrees>
			{
				auto vec = std::vector<Degrees>{};
				vec.resize(MaximumSpriteCount);
				return vec;
			}();

		auto Size() -> size_t
		{
			return Types.size();
		}
	};

	struct Player
	{
		SpriteType Sprite;
		Vector2 Position;
		Vector2 Velocity;
		Degrees Rotation;
	};

	struct EnemyDetails
	{
		EnemyType Type =  EnemyType::Common;
		bool Active = true;
		SpriteType Sprite = SpriteType::Enemy;
		Vector2 Position = {};
		Vector2 Velocity = {};
		Degrees Rotation = 0;
	};

	struct EntityCollection
	{
		Player Player{};
		Enemies Enemies{};
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
