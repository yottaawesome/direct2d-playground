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

	constexpr auto MaximumSpriteCount = 5;
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
	};

	struct EntityCollection
	{
		std::vector<EntityType> Entities{ MaximumSpriteCount };
		std::vector<bool> Active =
			[] static -> std::vector<bool>
			{
				auto vec = std::vector<bool>{};
				vec.resize(MaximumSpriteCount);
				return  vec;
			}();
		std::vector<SpriteType> SpriteCollection{ MaximumSpriteCount };
		std::vector<Vector2> Positions{ MaximumSpriteCount };
		std::vector<Vector2> Velocities{ MaximumSpriteCount };
		std::vector<Degrees> Rotations =
			[] static -> std::vector<Degrees>
			{
				auto vec = std::vector<Degrees>{};
				vec.resize(MaximumSpriteCount);
				return vec;
			}();
		auto AddEntity(this auto&& self, const EntityDetails& details) -> std::size_t
		{
			for (auto i = 0ull; i < self.Entities.size(); ++i)
			{
				if (self.Active[i])
					continue;
				self.Entities[i] = details.Type;
				self.Active[i] = details.Active;
				self.SpriteCollection[i] = details.Sprite;
				self.Positions[i] = details.Position;
				self.Velocities[i] = details.Velocity;
				self.Rotations[i] = details.Rotation;
				return i;
			}
			throw std::runtime_error{ "Maximum entity count reached" };
		}
	};
}
