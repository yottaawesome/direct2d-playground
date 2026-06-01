export module spacedefender:mainapp;
import std;
import shared;
import :assetmanager;
import :entity;
import :spacedefenderwindow;
import :input;
import :renderer;

export namespace SpaceDefender
{
	class MainApp final : public Shared::D2DApp
	{
	#pragma region Public
	public:
		// There's an MSVC modules bug that causes the Shared::GameMainWindow::OnEvent 
		// initialisation to fail if we don't provide a user-defined constructor here.
		MainApp() 
		{
			renderer.CreateResources();
			assetManager.Load(renderer.GetD2DDeviceContext());
			PositionPlayer();
			AddEnemies();
		}

		void OnResize(this auto&& self, std::uint32_t width, std::uint32_t height)
		{
			self.renderer.OnResize(width, height);
			self.window.Invalidate();
		}

		void OnIdle(this auto&& self)
		{
			self.Update(self.timer.Advance());
			self.DrawScene();
		}

		void Update(this auto&& self, float deltaTime)
		{
			if (self.inputState.IsPressed(Win32::Keys::Left))
			{
				self.entities.Player.Position.X -= 10.0f;
				if (self.entities.Player.Position.X < 0)
					self.entities.Player.Position.X = 0;
			}
			if (self.inputState.IsPressed(Win32::Keys::Right))
			{
				self.entities.Player.Position.X += 10.0f;
				auto [width, height] = self.assetManager[SpriteType::Player].GetSize();
				auto clientWidth = self.window.GetClientRect().right;
				if (self.entities.Player.Position.X + width > clientWidth)
					self.entities.Player.Position.X = clientWidth - width;
			}
			if (self.inputState.IsPressed(Win32::Keys::Space))
			{
				// shoot
			}

			constexpr auto enemyYSpeed = 10.0f; // pixels per second
			auto distance = enemyYSpeed * deltaTime;
			for (int x = 0; x < self.entities.Enemies.Size(); ++x)
			{
				if (not self.entities.Enemies.Active[x])
					continue;
				self.entities.Enemies.Positions[x].Y += distance;
			}
		}

		void DrawScene(this auto&& self)
		{
			self.renderer.CreateResources();
			self.assetManager.Load(self.renderer.GetD2DDeviceContext());
			if (self.window.IsIconic())
				return;

			static auto toDraw = std::vector<ToDraw>{};
			toDraw.clear();
			toDraw.push_back(ToDraw{
				.Sprite = self.assetManager[self.entities.Player.Sprite],
				.Position = self.entities.Player.Position,
				.Rotation = self.entities.Player.Rotation
			});

			for (auto i = 0; i < self.entities.Enemies.Size(); ++i)
			{
				if (not self.entities.Enemies.Active[i])
					continue;
				toDraw.push_back(ToDraw{
					.Sprite = self.assetManager[self.entities.Enemies.SpriteCollection[i]],
					.Position = self.entities.Enemies.Positions[i],
					.Rotation = self.entities.Enemies.Rotations[i]
				});
			}

			if (not self.renderer.DrawScene(toDraw, self.assetManager.SolidColorBrushes.DebugBrush.Get()))
			{
				self.assetManager.Discard();
				self.renderer.DiscardResources();
				Win32::ValidateRect(self.window.GetHandle(), nullptr);
			}
		}
	#pragma endregion

	#pragma region Private members
	private:
		Shared::Timer timer{};
		EntityCollection entities{};
		MainWindow window{
			MainWindow::OnEvent{
				.Render =
					[this]{ OnIdle(); },
				.Resize =
					[this](std::uint32_t width, std::uint32_t height) { OnResize(width, height); },
				.KeyUp = 
					[this](Win32::WPARAM key) { HandleKeyUp(key); },
				.KeyDown = 
					[this](Win32::WPARAM key) { HandleKeyDown(key); }
			}
		};
		Renderer renderer{ window.ToSurface() };
		AssetManager assetManager{};
		InputState inputState{};

		void LoadAssets(this MainApp& self)
		{
			self.assetManager.Load(self.renderer.GetD2DDeviceContext());
		}

		void PositionPlayer(this MainApp& self)
		{
			// The player belongs at the bottom of the screen, horizontally centered.
			auto rect = Win32::RECT{ self.window.GetClientRect() };
			auto [width, height] = self.assetManager[SpriteType::Player].GetSize();
			auto playerX = rect.bottom - height;
			auto playerY = rect.right / 2.f - width / 2.f;

			self.entities.Player = Player{
				.Sprite = SpriteType::Player,
				.Position = Shared::Vector2{ playerY, playerX },
				.Velocity = Shared::Vector2{ 0.0f, 0.0f },
				.Rotation = 0
			};
		}

		void AddEnemies(this MainApp& self)
		{
			auto clientRect = self.window.GetClientRect();
			auto topOfWindow = clientRect.top;
			auto [spriteWidth, spriteHeight] = self.assetManager[SpriteType::Enemy].GetSize();

			auto toSpawn = 4;
			auto rowWidth = spriteWidth * toSpawn;

			auto start = (clientRect.right - rowWidth) / 2;

			for (auto i = 0; i < toSpawn; ++i)
			{
				auto x = start + spriteWidth * (i);
				auto y = 0;
				self.entities.AddEnemy(
					EnemyDetails{
						.Type = EnemyType::Common,
						.Active = true,
						.Sprite = SpriteType::Enemy,
						.Position = Shared::Vector2{ static_cast<float>(x), static_cast<float>(y) },
						.Velocity = Shared::Vector2{ 0.0f, 0.0f },
						.Rotation = 0
					});
			}
		}

		#pragma region Input handling
		void HandleKeyDown(this auto&& self, Win32::WPARAM key)
		{
			self.inputState.WasPressed(key);
		}

		void HandleKeyUp(this auto&& self, Win32::WPARAM key)
		{
			self.inputState.WasReleased(key);
		}
		#pragma endregion
		#pragma endregion
	};
}