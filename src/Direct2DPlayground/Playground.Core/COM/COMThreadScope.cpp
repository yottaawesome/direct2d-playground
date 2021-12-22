module;

#include <stdexcept>
#include <Windows.h>
#include <Objbase.h>

module core.com.comthreadscope;
import core.error.comerror;

namespace Core::COM
{
	COMThreadScope::~COMThreadScope()
	{
		CoUninitialize();
	}

	COMThreadScope::COMThreadScope()
	{
		// https://docs.microsoft.com/en-us/windows/win32/api/combaseapi/nf-combaseapi-coinitializeex
		switch (const HRESULT hr = CoInitializeEx(nullptr, COINIT_MULTITHREADED))
		{
			case S_OK:
			case S_FALSE:
				break;
			default:
				throw Error::ComError(__FUNCSIG__": CoInitializeEx() failed", hr);
		}
	}
}