export module shared:window;
import std;
import :win32;
import :error;

namespace Shared
{
	struct HwndDeleter
	{
		static void operator()(Win32::HWND hwnd) noexcept
		{
			Win32::DestroyWindow(hwnd);
		}
	};
	using HwndUniquePtr = std::unique_ptr<std::remove_pointer_t<Win32::HWND>, HwndDeleter>;
}

export namespace Shared
{
	template<Win32::UINT VMessageId>
	struct TypedMessage
	{
		static constexpr auto MessageId = VMessageId;
		Win32::HWND Handle{};
		Win32::WPARAM WParam{};
		Win32::LPARAM LParam{};
	};

	namespace Messages
	{
		using Destroy = TypedMessage<Win32::Messages::Destroy>;
		using Paint = TypedMessage<Win32::Messages::Paint>;
		using KeyUp = TypedMessage<Win32::Messages::KeyUp>;
		using Command = TypedMessage<Win32::Messages::Command>;
		using NonClientDestroy = TypedMessage<Win32::Messages::NonClientDestroy>;
		using Size = TypedMessage<Win32::Messages::Size>;
		using DisplayChange = TypedMessage<Win32::Messages::DisplayChange>;
		using DpiChanged = TypedMessage<Win32::Messages::DpiChanged>;
	}

	constexpr auto HandledMessages = std::tuple{
		Messages::Destroy{},
		Messages::Paint{},
		Messages::KeyUp{},
		Messages::Command{},
		Messages::NonClientDestroy{},
		Messages::Size{},
		Messages::DisplayChange{},
		Messages::DpiChanged{}
	};
}

export namespace Shared
{
	template<typename TWindow>
	class Window
	{
	public:
		constexpr Window(bool init) 
		{ 
			init ? Init() : void();
		};

		constexpr Window(const Window&) = delete;
		constexpr auto operator=(const Window&) -> Window& = delete;

		Window(Window&& other) noexcept
		{
			m_window = std::move(other.m_window);
			if (m_window)
				Win32::SetWindowLongPtrW(m_window.get(), Win32::Gwlp::UserData, reinterpret_cast<Win32::LONG_PTR>(static_cast<TWindow*>(this)));
		}
		auto operator=(Window&& other) noexcept -> Window&
		{
			if (&other == this)
				return *this;
			if (m_window)
				Win32::SetWindowLongPtrW(other.GetHandle(), Win32::Gwlp::UserData, 0);   
			m_window = std::move(other.m_window);
			if (m_window)
				Win32::SetWindowLongPtrW(m_window.get(), Win32::Gwlp::UserData, reinterpret_cast<Win32::LONG_PTR>(this));
			return *this;
		}

		constexpr auto GetHandle() const -> Win32::HWND
		{
			return m_window.get();
		}

		auto GetClientRect(this auto&& self) -> Win32::RECT
		{
			if (not self.GetHandle())
				throw Error{ "Window handle is null" };
			auto rc = Win32::RECT{};
			if (not Win32::GetClientRect(self.GetHandle(), &rc))
				throw Win32Error{ Win32::GetLastError(), "Failed to get client rect" };
			return rc;
		}

		auto GetDpi(this auto&& self) -> std::uint32_t
		{
			return Win32::GetDpiForWindow(self.GetHandle());
		}

		auto Invalidate(this auto&& self, const Win32::RECT* rect = nullptr, bool erase = false)
		{
			if (not Win32::InvalidateRect(self.GetHandle(), rect, erase))
				throw Win32Error{ Win32::GetLastError(), "Failed to invalidate rect" };
		}

	protected:
		void Init(this auto&& self)
		{
			self.RegisterClass();
			self.CreateAndShowWindow();
		}

		// Each derived type registers under its own window class name; sharing
		// a single name would cause the second-and-later RegisterClassExW calls
		// to fail with ERROR_CLASS_ALREADY_EXISTS and silently fall back to the
		// first-registered class's WndProc.
		static auto ClassName() noexcept -> const wchar_t*
		{
			static const auto name = 
				[] static -> std::wstring
				{
					auto raw = std::string_view{ typeid(TWindow).name() };
					return { raw.begin(), raw.end() };
				}();
			return name.c_str();
		}

		static auto WndProc(
			Win32::HWND handle, 
			Win32::UINT msg, 
			Win32::WPARAM wParam, 
			Win32::LPARAM lParam
		) -> Win32::LRESULT
		{
			TWindow* pThis = nullptr;

			if (msg == Win32::Messages::NonClientCreate)
			{
				auto* pCreate = reinterpret_cast<Win32::CREATESTRUCT*>(lParam);
				pThis = reinterpret_cast<TWindow*>(pCreate->lpCreateParams);
				Win32::SetWindowLongPtrW(handle, Win32::Gwlp::UserData, reinterpret_cast<Win32::LONG_PTR>(pThis));

				// Adopt ownership once, during creation
				pThis->m_window = Shared::HwndUniquePtr(handle);
			}
			else
			{
				pThis = reinterpret_cast<TWindow*>(Win32::GetWindowLongPtrW(handle, Win32::Gwlp::UserData));
				// Detach before the OS destroys the HWND to avoid double-destroy later.
				// Caught by AI, more information here https://learn.microsoft.com/en-us/cpp/mfc/tn017-destroying-window-objects?view=msvc-170
				// under the "Auto cleanup with CWnd::PostNcDestroy" header.
				if (pThis and msg == Win32::Messages::NonClientDestroy)
				{
					Win32::SetWindowLongPtrW(handle, Win32::Gwlp::UserData, 0);
					(void)pThis->m_window.release();
				}
			}

			if (msg == Win32::Messages::Destroy)
			{
				Win32::PostQuitMessage(0);
				return 0;
			}

			if (not pThis)
				return Win32::DefWindowProcW(handle, msg, wParam, lParam);

			return [pThis, handle, wParam, lParam, msg]<std::size_t...Is>(std::index_sequence<Is...>)
			{
				auto result = Win32::LRESULT{};
				auto handled = 
					(... or [&result, pThis, handle, wParam, lParam, msg]
					{
						using MessageType = std::tuple_element_t<Is, decltype(HandledMessages)>;
						if constexpr (requires { pThis->OnMessage(MessageType{}); })
							if (msg == MessageType::MessageId)
								return (result = pThis->OnMessage(MessageType{ handle, wParam, lParam }), true);
						return false;
					}());
				return handled ? result : Win32::DefWindowProcW(handle, msg, wParam, lParam);
			}(std::make_index_sequence<std::tuple_size_v<decltype(HandledMessages)>>{});
		};

		auto GetWindowClassStruct(this auto&& self) noexcept -> Win32::WNDCLASSEXW
		{
			return Win32::WNDCLASSEXW{
				.cbSize = sizeof(Win32::WNDCLASSEXW),
				.style = 0,
				.lpfnWndProc = &WndProc,
				.cbClsExtra = 0,
				.cbWndExtra = 0,
				.hInstance = Win32::GetModuleHandleW(nullptr),
				.hIcon = Win32::LoadIconW(nullptr, Win32::IdiApplication),
				.hCursor = Win32::LoadCursorW(nullptr, Win32::IdcArrow),
				.hbrBackground = static_cast<Win32::HBRUSH>(Win32::GetStockObject(Win32::Brushes::White)),
				.lpszMenuName = nullptr,
				.lpszClassName = ClassName(),
			};
		}

		void RegisterClass(this auto&& self)
		{
			auto windowClass = Win32::WNDCLASSEXW{ self.GetWindowClassStruct() };
			if (not Win32::RegisterClassExW(&windowClass))
				if (auto lastError = Win32::GetLastError(); lastError != Win32::Error::ClassAlreadyExists)
					throw Win32Error{ lastError, "Failed to register window class" };
		}

		void CreateAndShowWindow(this auto&& self)
		{
			auto hwnd =
				Win32::CreateWindowExW(
					0,
					ClassName(),
					L"Shared Window",
					Win32::WindowStyles::OverlappedWindow | Win32::WindowStyles::Visible,
					0,
					0,
					800,
					600,
					nullptr,
					nullptr,
					Win32::GetModuleHandleW(nullptr),
					&self
				);
			if (not hwnd)
				throw Win32Error{ Win32::GetLastError(), "Failed to create window" };
		}

		Shared::HwndUniquePtr m_window;
	};
}
