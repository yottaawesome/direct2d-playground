module;

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <wincodec.h>
#include <wrl/client.h>

export module core:win32;
import std;

namespace Win32
{
	template<auto VValue>
	struct Win32Constant
	{
		operator decltype(VValue)(this auto&&) noexcept
			requires (not std::invocable<decltype(VValue)>)
		{
			return VValue;
		}

		operator decltype(VValue)(this auto&&) noexcept
			requires std::invocable<decltype(VValue)>
		{
			return VValue();
		}
	};
}

export namespace Win32
{
	using
		::CoCreateInstance,
		::MSG,
		::UINT64,
		::WNDCLASSEX,
		::RECT,
		::LONG_PTR,
		::LPCREATESTRUCT,
		::DWORD,
		::HWND,
		::UINT,
		::WPARAM,
		::LPARAM,
		::LRESULT,
		::LARGE_INTEGER,
		::HRESULT,
		::IWICBitmapDecoder,
		::CLSCTX,
		::IID_IWICImagingFactory2,
		::CLSID_WICImagingFactory2,
		::IWICFormatConverter,
		::IWICImagingFactory2,
		::QueryPerformanceCounter,
		::QueryPerformanceFrequency,
		::GetWindowLongPtrW,
		::LoadCursorW,
		::DefWindowProcW,
		::PostQuitMessage,
		::SetWindowLongPtrW,
		::PeekMessageW,
		::TranslateMessage,
		::DispatchMessageW,
		::DestroyWindow,
		::RegisterClassExW,
		::GetDesktopWindow,
		::GetDpiForWindow,
		::AdjustWindowRect,
		::CreateWindowExW,
		::GetModuleHandleW,
		::GetLastError,
		::ShowWindow,
		::UpdateWindow,
		::GetMessageW,
		::SetWindowPos,
		::Microsoft::WRL::ComPtr
		;

	constexpr auto PmRemove = PM_REMOVE;
	constexpr auto WsOverlappedWindow = WS_OVERLAPPEDWINDOW;
	constexpr auto CwUseDefault = CW_USEDEFAULT;
	constexpr auto SwShowNormal = SW_SHOWNORMAL;
	constexpr Win32Constant<IDI_APPLICATION> IdiApplication;

	constexpr auto CsHRedraw = CS_HREDRAW;
	constexpr auto CsVRedraw = CS_VREDRAW;

	constexpr auto HwndTop = HWND_TOP;
	constexpr auto SwpNoMove = SWP_NOMOVE;
	constexpr auto GwlpUserData = GWLP_USERDATA;

	constexpr auto GenericRead = GENERIC_READ;

	namespace Messages
	{
		enum
		{
			Quit = WM_QUIT,
			Create = WM_CREATE,
			Destroy = WM_DESTROY
		};
	}

	constexpr auto HrFailed(HRESULT hr) noexcept { return FAILED(hr); }
}
