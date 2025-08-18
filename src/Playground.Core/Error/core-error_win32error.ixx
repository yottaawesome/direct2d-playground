module;

#include <Windows.h>

export module core:error_win32error;
import std;
import :error_functions;

export namespace Core::Error
{
	struct Win32Error : std::runtime_error
	{
		Win32Error(const char* msg)
			: std::runtime_error(msg),
			m_errorCode(0)
		{
			m_errorString = std::format("{} {}", msg, "(no win32 error code was provided)");
		}

		Win32Error(const char* msg, DWORD errorCode)
			: std::runtime_error(msg),
			m_errorCode(errorCode)
		{
			m_errorString = Error::TranslateErrorCode<std::string>(errorCode);
			m_errorString = std::format("{} (win32 code: {}, {:#X}): {}", msg, errorCode, errorCode, m_errorString);
		}

		Win32Error(const std::string& msg, DWORD errorCode)
			: std::runtime_error(msg),
			m_errorCode(errorCode)
		{
			m_errorString = Error::TranslateErrorCode<std::string>(errorCode);
			m_errorString = std::format("{} (win32 code: {}, {:#X}): {}", msg, errorCode, errorCode, m_errorString);
		}

		DWORD GetErrorCode() const noexcept
		{
			return m_errorCode;
		}

		const char* what() const noexcept
		{
			return m_errorString.c_str();
		}

	protected:
		DWORD m_errorCode;
		std::string m_errorString;
	};
}