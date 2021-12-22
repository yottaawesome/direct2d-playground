module;

#include <string>
#include <stdexcept>
#include <Windows.h>

module core.error.comerror;

namespace Core::Error
{
	COMError::~COMError() {}
	COMError::COMError(const char* msg) : Win32Error(msg) {}
	COMError::COMError(const char* msg, const DWORD errorCode) : Win32Error(msg, errorCode) {}
	COMError::COMError(const std::string& msg, const DWORD errorCode) : Win32Error(msg, errorCode) {}
}