export module shared:ui.windowsurface;
import std;
import :com;
import :win32;
import :error;

export namespace Shared
{
	struct WindowSurface
	{
		Win32::HWND Hwnd{};
		std::uint32_t Dpi{};
		Win32::RECT Client{};

		[[nodiscard]]
		auto GetClientRect(this auto&& self) -> Win32::RECT
		{
			if (not self.Hwnd)
				return {};
			auto rc = Win32::RECT{};
			if (not Win32::GetClientRect(self.Hwnd, &rc))
				throw Win32Error{ Win32::GetLastError(), "Failed to get client rect" };
			return rc;
		}

		[[nodiscard]]
		auto GetDpi(this auto&& self) -> std::uint32_t
		{
			if (not self.Hwnd)
				return 0;
			auto dpi = Win32::GetDpiForWindow(self.Hwnd);
			if (dpi == 0)
				throw Win32Error{ Win32::GetLastError(), "Failed to get DPI for window" };
			return dpi;
		}

		[[nodiscard]]
		auto IsIconic(this auto&& self) -> bool
		{
			return self.Hwnd ? Win32::IsIconic(self.Hwnd) : false;
		}
	};
}