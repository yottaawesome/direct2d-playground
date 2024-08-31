module;

#include <stdexcept>
#include <format>
// See #include <winnt.h> for FAST_FAIL error codes
#include "Windows.h"

module core.error.win32error;
import core.error.functions;

namespace Core::Error
{
	Win32Error::~Win32Error() {}

	Win32Error::Win32Error(const char* msg)
		: std::runtime_error(msg),
		m_errorCode(0)
	{
		m_errorString = std::format("{} {}", msg, "(no win32 error code was provided)");
	}

	Win32Error::Win32Error(const char* msg, const DWORD errorCode)
		: std::runtime_error(msg),
		m_errorCode(errorCode)
	{
		m_errorString = Error::TranslateErrorCode<std::string>(errorCode);
		m_errorString = std::format("{} (win32 code: {}, {:#X}): {}", msg, errorCode, errorCode, m_errorString);
	}

	Win32Error::Win32Error(const std::string& msg, const DWORD errorCode)
		: std::runtime_error(msg),
		m_errorCode(errorCode)
	{
		m_errorString = Error::TranslateErrorCode<std::string>(errorCode);
		m_errorString = std::format("{} (win32 code: {}, {:#X}): {}", msg, errorCode, errorCode, m_errorString);
	}

	DWORD Win32Error::GetErrorCode() const noexcept
	{
		return m_errorCode;
	}

	const char* Win32Error::what() const noexcept
	{
		return m_errorString.c_str();
	}
}
