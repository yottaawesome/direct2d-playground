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
	auto app = SpaceDefender::MainApp{};
	return app.MainLoop();
}
catch (const Shared::Error& err)
{
	auto exitCode = err.ExitCode.value_or(Shared::ErrorExitCode::GeneralFailure);
	Shared::ErrorMessageBox(err.what(), "Error");
	return static_cast<int>(exitCode);
}
catch (const std::exception& ex)
{
	Shared::ErrorMessageBox(ex.what(), "Exception");
	return 1;
}
catch (...)
{
	Shared::ErrorMessageBox("An unknown error occurred.", "Unknown exception");
	return 1;
}