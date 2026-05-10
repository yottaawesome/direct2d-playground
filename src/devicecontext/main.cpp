import std;
import shared;
import devicecontextsample;

#pragma comment(lib, "d2d1.lib")
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "ole32.lib")

auto main() -> int
try
{
	auto com = Shared::ComApartment{};

	auto ctx = Win32::GetThreadDpiAwarenessContext();
	if (not Win32::AreDpiAwarenessContextsEqual(ctx, Win32::DpiAwarenessContext::PerMonitorAwareV2))
		throw Shared::Error{ std::string{ "Expected the DPI awareness context to be PerMonitorAwareV2" } };

	DeviceContextSample::Run();
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
