export module shared:d2dapp;
import std;
import :win32;

export namespace Shared
{
	class D2DApp
	{
	public:
		D2DApp() = default;

		auto GetHwnd(this auto&& self) noexcept -> Win32::HWND
		{
			return nullptr;
		}

		auto MainLoop(this auto&& self) -> int
		{
			auto msg = Win32::MSG{};
			while (msg.message != Win32::Messages::Quit)
			{
				if (Win32::PeekMessageW(&msg, self.GetHwnd(), 0, 0, Win32::PeekMessageFlags::Remove))
				{
					Win32::TranslateMessage(&msg);
					Win32::DispatchMessageW(&msg);
				}
				else
				{
					self.OnIdle();
				}
			}
			return 0;
		}

		void OnIdle(this auto&& self)
		{
		}
	};
}
