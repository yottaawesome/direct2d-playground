import std;
import shared;

#pragma comment(lib, "d2d1.lib")
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "Dwrite.lib")
#pragma comment(lib, "Windowscodecs.lib")

class TestApp final : public Shared::D2DApp
{
public:
	auto OnIdle(this auto&& self) -> void
	{
		self.deviceContext.CreateResources();
		if (not self.window.IsIconic())
			self.window.On.Render();

		self.deviceContext.BeginDraw();
		self.deviceContext->Clear(D2D1::ColorF(D2D1::ColorF::DarkSeaGreen));
		auto hr = Shared::HResult{self.deviceContext->EndDraw()};

		if (hr)
		{
			hr = self.deviceContext.Present();
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
	Shared::GameMainWindow window{ Shared::Init };
	Shared::DeviceContext deviceContext{ window.ToSurface() };
};

auto main() -> int
{
	auto app = TestApp{};
	return app.MainLoop();
}
