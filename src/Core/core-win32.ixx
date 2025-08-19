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
		auto Get() const noexcept
		{
			if constexpr (std::invocable<decltype(VValue)>)
				return std::invoke(VValue);
			else
				return VValue;
		}

		operator std::invoke_result_t<decltype(&Win32Constant::Get), Win32Constant>() const noexcept
		{
			return Get();
		}
	};
}

struct F
{
	operator bool() { return false; }
};

export namespace D2D1
{
	using
		::D2D1::SizeU,
		::D2D1::RenderTargetProperties,
		::D2D1::HwndRenderTargetProperties,
		::D2D1::Matrix3x2F,
		::D2D1::ColorF,
		::D2D1::RectF
		;
}

export namespace Win32
{
	using
		::IDWriteTextFormat,
		::ID2D1SolidColorBrush,
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
		::D2D1_RECT_F,
		::IWICBitmapDecoder,
		::HMODULE,
		::CLSCTX,
		::D2D1_FACTORY_TYPE,
		::DWRITE_FACTORY_TYPE,
		::IID_IWICImagingFactory2,
		::CLSID_WICImagingFactory2,
		::IWICFormatConverter,
		::ID2D1HwndRenderTarget,
		::IWICImagingFactory2,
		::COINIT,
		::ID2D1Factory,
		::IDWriteFactory,
		::DWRITE_FONT_STRETCH,
		::DWRITE_FONT_WEIGHT,
		::DWRITE_FONT_STYLE,
		::DWRITE_TEXT_ALIGNMENT,
		::DWRITE_PARAGRAPH_ALIGNMENT,
		::Microsoft::WRL::ComPtr,
		::GetDpiForWindow,
		::GetDesktopWindow,
		::DWriteCreateFactory,
		::CoUninitialize,
		::CoInitializeEx,
		::FormatMessageA,
		::FormatMessageW,
		::LocalFree,
		::QueryPerformanceCounter,
		::QueryPerformanceFrequency,
		::GetWindowLongPtrW,
		::CoCreateInstance,
		::LoadCursorW,
		::DefWindowProcW,
		::D2D1CreateFactory,
		::PostQuitMessage,
		::SetWindowLongPtrW,
		::PeekMessageW,
		::LoadLibraryW,
		::FreeLibrary,
		::GetClientRect,
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
		::InvalidateRect,
		::ValidateRect
		;

	namespace D2DError
	{
		enum
		{
			RecreateTarget = D2DERR_RECREATE_TARGET
		};
	}
	constexpr Win32Constant<[]{ return CLSID_WICImagingFactory2; }> ClsWICImagingFactory2;

	constexpr auto LoWord(auto v) noexcept -> WORD { return LOWORD(v); }
	constexpr auto HiWord(auto v) noexcept -> WORD { return HIWORD(v); }

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
			Destroy = WM_DESTROY,
			Size = WM_SIZE,
			DisplayChange = WM_DISPLAYCHANGE,
			Paint = WM_PAINT,
		};
	}

	constexpr auto HrFailed(HRESULT hr) noexcept { return FAILED(hr); }

	constexpr auto SOk = S_OK;
	constexpr auto SFalse = S_FALSE;
}
