export module core:com_comthreadscope;
import std;
import :win32;
import :error_comerror;

export namespace COM
{
	struct COMThreadScope
	{
		~COMThreadScope()
		{
			Win32::CoUninitialize();
		}

		COMThreadScope()
		{
			// https://docs.microsoft.com/en-us/windows/win32/api/combaseapi/nf-combaseapi-coinitializeex
			switch (Win32::HRESULT hr = Win32::CoInitializeEx(nullptr, Win32::COINIT::COINIT_MULTITHREADED))
			{
				case Win32::SOk:
				case Win32::SFalse:
					break;
				default:
					throw Error::COMError("CoInitializeEx() failed", hr);
			}
		}
	};
}
