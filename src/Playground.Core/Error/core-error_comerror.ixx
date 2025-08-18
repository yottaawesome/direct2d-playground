module;

#include <Windows.h>

export module core:error_comerror;
import std;
import :error_win32error;

export namespace Core::Error
{
	struct COMError : Win32Error
	{
		COMError(const char* msg) : Win32Error(msg) {}
		COMError(const char* msg, const DWORD errorCode) : Win32Error(msg, errorCode) {}
		COMError(const std::string& msg, const DWORD errorCode) : Win32Error(msg, errorCode) {}
	};
}
