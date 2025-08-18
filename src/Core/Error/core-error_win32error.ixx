export module core:error_win32error;
import std;
import :win32;
import :error_functions;

export namespace Error
{
	struct Win32Error : std::runtime_error
	{
		Win32Error(const char* msg)
			: std::runtime_error(msg),
			m_errorCode(0)
		{
			m_errorString = std::format("{} {}", msg, "(no win32 error code was provided)");
		}

		Win32Error(const char* msg, Win32::DWORD errorCode)
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

		Win32::DWORD GetErrorCode() const noexcept
		{
			return m_errorCode;
		}

		const char* what() const noexcept
		{
			return m_errorString.c_str();
		}

	protected:
		Win32::DWORD m_errorCode;
		std::string m_errorString;
	};
}