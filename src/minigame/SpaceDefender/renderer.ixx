export module spacedefender:renderer;
import std;
import shared;

export namespace SpaceDefender
{
	struct ToDraw
	{
		Shared::Bitmap& Sprite;
		Shared::Vector2 Position{};
		float Rotation = 0.0f;
	};

	class Renderer
	{
	public:
		struct Settings 
		{
			D2D1::ColorF ClearColor = D2D1::ColorF(D2D1::ColorF::DarkSeaGreen);
		};

		Settings settings;

		Renderer(Shared::WindowSurface windowSurface, Settings rendererSettings)
			: deviceContext(windowSurface), settings{rendererSettings}
		{}

		void CreateResources(this auto&& self)
		{
			self.deviceContext.CreateResources();
		}

		auto GetD2DDeviceContext(this auto&& self) -> D2D1::ID2D1DeviceContext*
		{
			return self.deviceContext.GetD2DDeviceContext();
		}

		auto OnResize(this auto&& self, std::uint32_t width, std::uint32_t height)
		{
			self.deviceContext.OnResize(width, height);
		}

		auto DrawScene(
			this auto&& self, 
			const std::vector<ToDraw>& toDraw, 
			D2D1::ID2D1SolidColorBrush* debugBrush
		) -> bool
		{
			self.deviceContext.BeginDraw();
			// Clear the background to the configured clear color
			self.deviceContext->SetTransform(D2D1::Matrix3x2F::Identity());
			self.deviceContext->Clear(self.settings.ClearColor);

			for (const ToDraw& item : toDraw)
			{
				// Apply the correct world transform for the sprite based on its position and rotation.
				auto [x, y] = item.Position;
				auto translation = D2D1::Matrix3x2F::Translation(x, y);
				auto rotation = D2D1::Matrix3x2F::Rotation(
					item.Rotation,
					item.Sprite.GetCenter()
				);
				auto transform = D2D1::Matrix3x2F{ rotation * translation };
				self.deviceContext->SetTransform(transform);

				// Determine the size of the sprite so we can draw it and its bounding box correctly.
				auto [width, height] = item.Sprite->GetSize();
				self.deviceContext->DrawBitmap(
					item.Sprite.Get(),
					D2D1::RectF(0, 0, width, height)
				);

				// This is just for debugging: draw a box around the sprite to show its bounds.
				self.deviceContext->DrawRectangle(
					D2D1::RectF(0, 0, width, height),
					debugBrush,
					5.0f,
					nullptr
				);
			}
			
			if (auto hr = Shared::HResult{ self.deviceContext.EndDraw() }; hr)
			{
				if (hr = self.deviceContext.Present(1, 0); not hr)
					throw Shared::ComError{ hr, "IDXGISwapChain::Present() failed" };
				return true;
			}
			else if (hr.Code == D2D1::Error::RecreateTarget)
			{
				return false;
			}
			else
			{
				throw Shared::ComError{ hr, "ID2D1DeviceContext::EndDraw() failed" };
			}
		}

		void DiscardResources(this auto&& self)
		{
			self.deviceContext.DiscardResources();
		}
	private:
		Shared::DeviceContext deviceContext;
	};
}