export module shared:comapartment;
import :win32;
import :error;

export namespace Shared
{
	struct ComApartment
	{
		~ComApartment()
		{
			Win32::CoUninitialize();
		}

		ComApartment()
		{
			auto hr = Shared::HResult{
				Win32::CoInitializeEx(
					nullptr,
					Win32::COINIT::COINIT_MULTITHREADED | Win32::COINIT::COINIT_DISABLE_OLE1DDE
				)};
			if (not hr)
				throw Shared::ComError{ hr, "CoInitializeEx() failed" };
		}
	};
}
