export module spacedefender:mainapp;
import std;
import shared;
import :assetmanager;
import :entity;

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

		Shared::Timer timer{};

		void Update(this auto&& self, float deltaTime)
		{
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

				auto [x, y] = self.entities.Positions[i];
				auto translation = D2D1::Matrix3x2F::Translation(x, y);
				auto rotation = D2D1::Matrix3x2F::Rotation(
					self.entities.Rotations[i],
					self.assetManager.Bitmaps[self.entities.SpriteCollection[i]].GetCenter()
				);
				auto transform = rotation*translation;
				self.deviceContext->SetTransform(transform);

				self.deviceContext->DrawBitmap(
					self.assetManager.Bitmaps[self.entities.SpriteCollection[i]].Get(),
					D2D1::RectF(100, 100, 0, 0)
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
		EntityCollection entities{};
		Shared::GameWindow window{
			Shared::GameWindow::OnEvent{
				.Render =
					[this]
					{
						OnIdle();
					},
				.Resize =
					[this](std::uint32_t width, std::uint32_t height)
					{
						OnResize(width, height);
					},
				.KeyUp = [this](Win32::WPARAM key)
					{
						if (key == Win32::Keys::Escape)
							Win32::PostMessageW(window.GetHandle(), Win32::Messages::Destroy, 0, 0);
					},
				.KeyDown =
					[this](Win32::WPARAM key)
					{
						if (key == Win32::Keys::Space)
						{
							// shoot
						}
						if (key == Win32::Keys::Left)
						{
							// move left
							entities.Positions[0].X -= 10.0f;
						}
						if (key == Win32::Keys::Right)
						{
							// move right
							entities.Positions[0].X += 10.0f;
						}
					}
			}
		};
		Shared::DeviceContext deviceContext{ window.ToSurface() };
		AssetManager assetManager{};

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
		#pragma endregion
	};
}