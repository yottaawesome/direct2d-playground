import std;
import shared;

#pragma comment(lib, "d2d1.lib")
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "Dwrite.lib")
#pragma comment(lib, "Windowscodecs.lib")

class TestApp final : public Shared::D2DApp
{
public:
	void OnResize(this auto&& self, std::uint32_t width, std::uint32_t height)
	{
		self.deviceContext.OnResize(width, height);
		self.window.Invalidate();
	}

	void OnIdle(this auto&& self)
	{
		self.deviceContext.CreateResources();
		if (self.window.IsIconic())
			return;

		self.deviceContext.BeginDraw();
		self.deviceContext->Clear(D2D1::ColorF(D2D1::ColorF::DarkSeaGreen));
		auto hr = Shared::HResult{self.deviceContext->EndDraw()};

		if (hr)
		{
			hr = self.deviceContext.Present(1, 0);
			if (not hr)
				throw Shared::ComError{ hr, "IDXGISwapChain::Present() failed" };
		}
		else if (hr.Code == D2D1::Error::RecreateTarget)
		{
			self.deviceContext.DiscardResources();
			Win32::ValidateRect(self.window.GetHandle(), nullptr);
		}
		else
		{
			throw Shared::ComError{ hr, "EndDraw() failed in OnIdle()" };
		}
	}

private:
	Shared::GameMainWindow window{ 
		Shared::GameMainWindow::OnEvent{
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
		} 
	};
	Shared::DeviceContext deviceContext{ window.ToSurface() };
};

auto main() -> int
{
	auto app = TestApp{};
	return app.MainLoop();
}
