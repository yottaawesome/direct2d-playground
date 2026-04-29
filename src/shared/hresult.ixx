export module shared:hresult;
import std;
import :win32;

export namespace Shared
{
	struct HResult 
	{
		Win32::HRESULT Code = 0;
		constexpr auto Succeeded() const noexcept { return Win32::Succeeded(Code); }
		constexpr auto Failed() const noexcept { return Win32::Failed(Code); }
		constexpr operator Win32::HRESULT() const noexcept { return Code; }
		constexpr bool operator==(Win32::HRESULT other) const noexcept { return Code == other; }
		constexpr operator bool() const noexcept { return Succeeded(); }
	};
}