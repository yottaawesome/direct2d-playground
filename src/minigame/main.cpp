import std;
import shared;

#pragma comment(lib, "d2d1.lib")
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "Dwrite.lib")
#pragma comment(lib, "Windowscodecs.lib")

auto main() -> int
{
	auto window = Shared::GameMainWindow{ Shared::Init };
	auto deviceContext = Shared::DeviceContext{ window.ToSurface() };

	auto app = Shared::D2DApp{ std::move(window) };
	return app.MainLoop();
}
