module;

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <Objbase.h>
#include <wincodec.h>
#include <d2d1.h>
#include <d2d1helper.h>
#include <dwrite.h>
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

export namespace D2D1
{
	using
		::D2D1::SizeU,
		::D2D1::RenderTargetProperties,
		::D2D1::HwndRenderTargetProperties
		;
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
		::UINT32,
		::D2D1_SIZE_U,
		::WPARAM,
		::LPARAM,
		::LRESULT,
		::LARGE_INTEGER,
		::HRESULT,
		::IWICBitmapDecoder,
		::HMODULE,
		::CLSCTX,
		::D2D1_FACTORY_TYPE,
		::IID_IWICImagingFactory2,
		::CLSID_WICImagingFactory2,
		::IWICFormatConverter,
		::ID2D1HwndRenderTarget,
		::IWICImagingFactory2,
		::COINIT,
		::ID2D1Factory,
		::CoUninitialize,
		::CoInitializeEx,
		::FormatMessageA,
		::FormatMessageW,
		::LocalFree,
		::QueryPerformanceCounter,
		::QueryPerformanceFrequency,
		::GetWindowLongPtrW,
		::LoadCursorW,
		::DefWindowProcW,
		::D2D1CreateFactory,
		::PostQuitMessage,
		::SetWindowLongPtrW,
		::PeekMessageW,
		::LoadLibraryW,
		::FreeLibrary,
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

	namespace FormatMessageFlags
	{
		constexpr auto AllocateBuffer = FORMAT_MESSAGE_ALLOCATE_BUFFER;
		constexpr auto FromSystem = FORMAT_MESSAGE_FROM_SYSTEM;
		constexpr auto IgnoreInserts = FORMAT_MESSAGE_FROM_SYSTEM;
		constexpr auto FromHModule = FORMAT_MESSAGE_FROM_SYSTEM;
	}

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

	constexpr auto SOk = S_OK;
	constexpr auto SFalse = S_FALSE;
}
