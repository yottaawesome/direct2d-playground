export module spacedefender:mainapp;
import std;
import shared;
import :assetmanager;
import :entity;
import :spacedefenderwindow;
import :input;

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
			deviceContext.CreateResources();
			assetManager.Load(deviceContext.GetD2DDeviceContext());
			PositionPlayer();
			AddEnemies();
		}

		void OnResize(this auto&& self, std::uint32_t width, std::uint32_t height)
		{
			self.deviceContext.OnResize(width, height);
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
				self.entities.Positions[0].X -= 10.0f;
				if (self.entities.Positions[0].X < 0)
					self.entities.Positions[0].X = 0;
			}
			if (self.inputState.IsPressed(Win32::Keys::Right))
			{
				self.entities.Positions[0].X += 10.0f;
				auto [width, height] = self.assetManager[SpriteType::Player].GetSize();
				auto clientWidth = self.window.GetClientRect().right;
				if (self.entities.Positions[0].X + width > clientWidth)
					self.entities.Positions[0].X = clientWidth - width;
			}
			if (self.inputState.IsPressed(Win32::Keys::Space))
			{
				// shoot
			}

			constexpr auto enemyYSpeed = 10.0f; // pixels per second
			auto distance = enemyYSpeed * deltaTime;
			for (int x = 0; x < self.entities.Entities.size(); ++x)
			{
				if (not self.entities.Active[x])
					continue;
				if (self.entities.Entities[x] == EntityType::Enemy)
				{
					self.entities.Positions[x].Y += distance;
				}
			}
		}

		void DrawScene(this auto&& self)
		{
			self.deviceContext.CreateResources();
			self.assetManager.Load(self.deviceContext.GetD2DDeviceContext());
			if (self.window.IsIconic())
				return;

			self.deviceContext.BeginDraw();
			self.deviceContext->SetTransform(D2D1::Matrix3x2F::Identity());
			self.deviceContext->Clear(D2D1::ColorF(D2D1::ColorF::DarkSeaGreen));

			for (auto i = 0; i < self.entities.Entities.size(); ++i)
			{
				if (not self.entities.Active[i])
					continue;

				// Apply the correct world transform for the sprite based on its position and rotation.
				auto [x, y] = self.entities.Positions[i];
				auto translation = D2D1::Matrix3x2F::Translation(x, y);
				auto rotation = D2D1::Matrix3x2F::Rotation(
					self.entities.Rotations[i],
					self.assetManager.Bitmaps[self.entities.SpriteCollection[i]].GetCenter()
				);
				auto transform = D2D1::Matrix3x2F{ rotation * translation };
				self.deviceContext->SetTransform(transform);

				// Determine the size of the sprite so we can draw it and its bounding box correctly.
				auto [width, height] = self.assetManager.Bitmaps[self.entities.SpriteCollection[i]].GetSize();
				self.deviceContext->DrawBitmap(
					self.assetManager.Bitmaps[self.entities.SpriteCollection[i]].Get(),
					D2D1::RectF(0, 0, width, height)
				);
				// This is just for debugging: draw a box around the sprite to show its bounds.
				self.deviceContext->DrawRectangle(
					D2D1::RectF(0, 0, width, height),
					self.assetManager.SolidColorBrushes.DebugBrush.Get(),
					5.0f,
					nullptr
				);
			}

			auto hr = Shared::HResult{ self.deviceContext->EndDraw() };

			if (hr)
			{
				hr = self.deviceContext.Present(1, 0);
				if (not hr)
					throw Shared::ComError{ hr, "IDXGISwapChain::Present() failed" };
			}
			else if (hr.Code == D2D1::Error::RecreateTarget)
			{
				self.assetManager.Discard();
				self.deviceContext.DiscardResources();
				Win32::ValidateRect(self.window.GetHandle(), nullptr);
			}
			else
			{
				throw Shared::ComError{ hr, "EndDraw() failed in OnIdle()" };
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
		Shared::DeviceContext deviceContext{ window.ToSurface() };
		AssetManager assetManager{};
		InputState inputState{};

		void LoadAssets(this MainApp& self)
		{
			self.assetManager.Load(self.deviceContext.GetD2DDeviceContext());
		}

		void PositionPlayer(this MainApp& self)
		{
			// The player belongs at the bottom of the screen, horizontally centered.
			auto rect = Win32::RECT{ self.window.GetClientRect() };
			auto [width, height] = self.assetManager[SpriteType::Player].GetSize();
			auto playerX = rect.bottom - height;
			auto playerY = rect.right / 2.f - width / 2.f;

			self.entities.AddEntity(
				EntityDetails{
					.Type = EntityType::Player,
					.Active = true,
					.Sprite = SpriteType::Player,
					.Position = Vector2{playerY, playerX},
					.Velocity = Vector2{ 0.0f, 0.0f },
					.Rotation = 0
				});
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
				self.entities.AddEntity(
					EntityDetails{
						.Type = EntityType::Enemy,
						.Active = true,
						.Sprite = SpriteType::Enemy,
						.Position = Vector2{ static_cast<float>(x), static_cast<float>(y) },
						.Velocity = Vector2{ 0.0f, 0.0f },
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