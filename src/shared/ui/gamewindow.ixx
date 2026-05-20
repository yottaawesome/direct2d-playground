export module shared:ui.gamewindow;
import std;
import :win32;
import :ui.window;
import :misc;

export namespace Shared
{
	class GameWindow final : public Window
	{
	public:
		using RenderFn = std::move_only_function<auto()->void>;
		using ResizeFn = std::move_only_function<auto(std::uint32_t, std::uint32_t)->void>;
		using DpiChangedFn = std::move_only_function<auto(std::uint32_t, const Win32::RECT&)->void>;
		using DisplayChangeFn = std::move_only_function<auto()->void>;
		using KeyUpFn = std::move_only_function<auto(Win32::WPARAM)->void>;
		using KeyDownFn = std::move_only_function<auto(Win32::WPARAM)->void>;

		struct OnEvent
		{
			RenderFn Render = [] {};
			ResizeFn Resize = [](std::uint32_t, std::uint32_t) {};
			DpiChangedFn DpiChanged = [](std::uint32_t, const Win32::RECT&) {};
			DisplayChangeFn DisplayChange = [] {};
			KeyUpFn KeyUp = [](Win32::WPARAM) {};
			KeyDownFn KeyDown = [](Win32::WPARAM) {};
		};

		GameWindow(GameWindow&&) noexcept = default;
		auto operator=(GameWindow&&) noexcept -> GameWindow & = default;

		GameWindow(NoInitTag) : Window{false}
		{}

		GameWindow(InitTag) : Window{ false }
		{
			Init();
		}

		GameWindow(OnEvent handlers)
			: Window{ false }
			, On{ std::move(handlers) }
		{
			// We need to Init() after setting the function objects, 
			// because the message handlers call those functions.
			// Letting the base constructor do the init would cause 
			// the handlers to call empty function objects, generating 
			// errors.
			Init();
		}

		OnEvent On {
			.Render = [] {},
			.Resize = [](std::uint32_t, std::uint32_t) {},
			.DpiChanged = [](std::uint32_t, const Win32::RECT&) {},
			.DisplayChange = [] {},
			.KeyUp = [](Win32::WPARAM) {}
		};

		// This is the minimum message handling needed to support rendering in the main loop. 
		// You can add handlers for other messages (e.g. WM_SIZE) as needed.
		auto OnMessage(this auto&& self, const Messages::Paint& msg) -> Win32::LRESULT
		{
			auto ps = Win32::PAINTSTRUCT{};
			Win32::BeginPaint(self.GetHandle(), &ps);
			self.On.Render();
			Win32::EndPaint(self.GetHandle(), &ps);
			return 0;
		}

		auto OnMessage(this auto&& self, const Messages::KeyUp& msg) -> Win32::LRESULT
		{
			self.On.KeyUp(msg.WParam);
			return 0;
		}

		auto OnMessage(this auto&& self, const Messages::KeyDown& msg) -> Win32::LRESULT
		{
			self.On.KeyDown(msg.WParam);
			return 0;
		}

		auto OnMessage(this auto&& self, const Messages::Size& msg) -> Win32::LRESULT
		{
			auto width = Win32::LoWord(msg.LParam);
			auto height = Win32::HiWord(msg.LParam);
			self.On.Resize(width, height);
			return 0;
		}
		
		auto OnMessage(this auto&& self, const Messages::DisplayChange& msg) -> Win32::LRESULT
		{
			Win32::InvalidateRect(self.GetHandle(), nullptr, false);
			self.On.DisplayChange();
			return 0;
		}
		
		auto OnMessage(this auto&& self, const Messages::DpiChanged& msg) -> Win32::LRESULT
		{
			const auto rect = reinterpret_cast<const Win32::RECT*>(msg.LParam);
			self.On.DpiChanged(self.GetDpi(), *rect);
			return 0;
		}
	};
}