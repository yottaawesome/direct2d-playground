import std;
import shared;
import spacedefender;

#pragma comment(lib, "d2d1.lib")
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "Dwrite.lib")
#pragma comment(lib, "Windowscodecs.lib")

auto main() -> int
try
{
	auto com = Shared::ComApartment{};
	auto wicContext = Shared::WicContext{};
	auto playerDecoder = wicContext.CreateDecoderFromFilename({ .Filename = L"assets\\player.png" });
	auto frame = playerDecoder.GetFrame(0);

	auto converter = wicContext.CreateConverter({
		.Source = frame.Get(),
		.DestinationFormat = WIC::GUID_WICPixelFormat32bppPBGRA
	});

	auto app = SpaceDefender::MainApp{};
	return app.MainLoop();
}
catch (const std::exception& ex)
{
	Win32::MessageBoxA(nullptr, ex.what(), "Error", Win32::MessageBoxOptions::Ok | Win32::MessageBoxOptions::Critical);
	return 1;
}
catch (...)
{
	Win32::MessageBoxA(nullptr, "An unknown error occurred.", "Error", Win32::MessageBoxOptions::Ok | Win32::MessageBoxOptions::Critical);
	return 1;
}