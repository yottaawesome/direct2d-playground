export module core:error_comerror;
import std;
import :win32;
import :error_win32error;

export namespace Error
{
	struct COMError : Win32Error
	{
		COMError(const char* msg) : Win32Error(msg) {}
		COMError(const char* msg, Win32::DWORD errorCode) : Win32Error(msg, errorCode) {}
		COMError(const std::string& msg, Win32::DWORD errorCode) : Win32Error(msg, errorCode) {}
	};
}
