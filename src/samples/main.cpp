import std;
import shared;
import samples;

#pragma comment(lib, "d2d1.lib")
#pragma comment(lib, "Dwrite.lib")
#pragma comment(lib, "Windowscodecs.lib")

auto main() -> int
try
{
	Win32::CoInitializeEx(nullptr, Win32::COINIT::COINIT_MULTITHREADED | Win32::COINIT::COINIT_DISABLE_OLE1DDE);

	// Prefer manifest-based DPI awareness, the vcxproj setting is less reliable.
	auto ctx = Win32::GetThreadDpiAwarenessContext();
	// You cannot directly compare DPI_AWARENESS_CONTEXT values.
	if (not Win32::AreDpiAwarenessContextsEqual(ctx, Win32::DpiAwarenessContext::PerMonitorAwareV2))
		throw Shared::Error{std::string{"Expected the DPI awareness context to be PerMonitorAwareV2"}};

	//Rectangles::Run();
	GameLoop::Run();

	return 0;
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
