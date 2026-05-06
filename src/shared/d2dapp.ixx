export module shared:d2dapp;
import std;
import :win32;

export namespace Shared
{
	template<typename TWindow>
	class D2DApp final
	{
	public:
		D2DApp(TWindow&& window) : window(std::forward<TWindow>(window))
		{ }

		auto MainLoop(this auto&& self) -> int
		{
			auto msg = Win32::MSG{};
			while (msg.message != Win32::Messages::Quit)
			{
				if (Win32::PeekMessageW(&msg, self.window.GetHandle(), 0, 0, Win32::PeekMessageFlags::Remove))
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
	private:
		TWindow window;
	};
}
