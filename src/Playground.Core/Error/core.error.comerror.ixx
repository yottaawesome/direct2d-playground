module;

#include <string>
#include <stdexcept>
#include <Windows.h>

export module core.error.comerror;
import core.error.win32error;

export namespace Core::Error
{
	class COMError : public Win32Error
	{
		public:
			virtual ~COMError();
			COMError(const char* msg);
			COMError(const char* msg, const DWORD errorCode);
			COMError(const std::string& msg, const DWORD errorCode);
	};
}