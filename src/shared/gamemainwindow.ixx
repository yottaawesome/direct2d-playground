export module shared:gamemainwindow;
import std;
import :win32;
import :window;

export namespace Shared
{
	class GameMainWindow final : public Window<GameMainWindow>
	{
	public:
		using RenderFn = std::move_only_function<auto()->void>;
		using ResizeFn = std::move_only_function<auto(std::uint32_t, std::uint32_t)->void>;
		using DpiChangedFn = std::move_only_function<auto(std::uint32_t, const Win32::RECT&)->void>;

		RenderFn OnRender = [] {};
		ResizeFn OnResize = [](std::uint32_t, std::uint32_t) {};
		DpiChangedFn OnDpiChanged = [](std::uint32_t, const Win32::RECT&) {};

		GameMainWindow(GameMainWindow&&) noexcept = default;
		auto operator=(GameMainWindow&&) noexcept -> GameMainWindow& = default;

		GameMainWindow() : Window<GameMainWindow>{false}
		{}

		GameMainWindow(
			RenderFn onRender,
			ResizeFn onResize,
			DpiChangedFn onDpiChanged
		) : Window<GameMainWindow>{ false }
			, OnRender(std::move(onRender))
			, OnResize(std::move(onResize))
			, OnDpiChanged(std::move(onDpiChanged))
		{
			// We need to Init() after setting the function objects, 
			// because the message handlers call those functions.
			// Letting the base constructor do the init would cause 
			// the handlers to call empty function objects, generating 
			// errors.
			Init();
		}

		// This is the minimum message handling needed to support rendering in the main loop. 
		// You can add handlers for other messages (e.g. WM_SIZE) as needed.
		auto OnMessage(this auto&& self, const Messages::Paint& msg) -> Win32::LRESULT
		{
			auto ps = Win32::PAINTSTRUCT{};
			Win32::BeginPaint(self.GetHandle(), &ps);
			self.OnRender();
			Win32::EndPaint(self.GetHandle(), &ps);
			return 0;
		}
		auto OnMessage(this auto&& self, const Messages::Size& msg) -> Win32::LRESULT
		{
			auto width = Win32::LoWord(msg.LParam);
			auto height = Win32::HiWord(msg.LParam);
			self.OnResize(width, height);
			return 0;
		}
		auto OnMessage(this auto&& self, const Messages::DisplayChange& msg) -> Win32::LRESULT
		{
			const auto dpi = std::uint32_t{ Win32::HiWord(msg.WParam) };
			const auto rect = reinterpret_cast<const Win32::RECT*>(msg.LParam);
			self.OnDpiChanged(dpi, *rect);
			return 0;
		}
		auto OnMessage(this auto&& self, const Messages::DpiChanged& msg) -> Win32::LRESULT
		{
			Win32::InvalidateRect(self.GetHandle(), nullptr, false);
			return 0;
		}
	};
}