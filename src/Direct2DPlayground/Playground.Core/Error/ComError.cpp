module;

#include <string>
#include <stdexcept>
#include <Windows.h>

module core.error.comerror;

namespace Core::Error
{
	ComError::~ComError() {}
	ComError::ComError(const char* msg) : Win32Error(msg) {}
	ComError::ComError(const char* msg, const DWORD errorCode) : Win32Error(msg, errorCode) {}
	ComError::ComError(const std::string& msg, const DWORD errorCode) : Win32Error(msg, errorCode) {}
}