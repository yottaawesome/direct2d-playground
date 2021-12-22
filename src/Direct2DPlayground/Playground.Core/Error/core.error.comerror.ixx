module;

#include <string>
#include <stdexcept>
#include <Windows.h>

export module core.error.comerror;
import core.error.win32error;

export namespace Core::Error
{
	class ComError : public Win32Error
	{
		public:
			virtual ~ComError();
			ComError(const char* msg);
			ComError(const char* msg, const DWORD errorCode);
			ComError(const std::string& msg, const DWORD errorCode);
	};
}