module;

#include <stdexcept>
#include <Windows.h>
#include <Objbase.h>

export module core:com_comthreadscope;
import :error_comerror;

export namespace Core::COM
{
	struct COMThreadScope
	{
		~COMThreadScope()
		{
			CoUninitialize();
		}

		COMThreadScope()
		{
			// https://docs.microsoft.com/en-us/windows/win32/api/combaseapi/nf-combaseapi-coinitializeex
			switch (const HRESULT hr = CoInitializeEx(nullptr, COINIT_MULTITHREADED))
			{
				case S_OK:
				case S_FALSE:
					break;
				default:
					throw Error::COMError(__FUNCSIG__": CoInitializeEx() failed", hr);
			}
		}
	};
}
