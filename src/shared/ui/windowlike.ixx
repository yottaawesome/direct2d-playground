export module shared:windowlike;
import std;
import :win32;

export namespace Shared
{
	template<typename T>
	concept WindowLike = requires(T t)
	{
		{ t.GetHandle() } -> std::same_as<Win32::HWND>;
		{ t.GetDpi() } -> std::same_as<std::uint32_t>;
		{ t.GetClientRect() } -> std::same_as<Win32::RECT>;
	};
}