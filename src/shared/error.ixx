export module shared:error;
import std;
import :win32;

export namespace Shared
{
	auto SystemCodeToString(Win32::DWORD code) -> std::string
	{
		void* buffer;
		auto chars = 
			Win32::FormatMessageA(
				Win32::FormatMessageFlags::FromSystem | Win32::FormatMessageFlags::AllocateBuffer,
				nullptr,
				code,
				0,
				reinterpret_cast<LPSTR>(&buffer),
				0,
				nullptr
			);
		if (chars == 0)
			return std::format("Unknown error code: {}", code);

		auto message = std::string(static_cast<char*>(buffer), chars); 
		Win32::LocalFree(buffer);
		return message;
	}

	struct SystemCode
	{
		Win32::DWORD Code = 0;
		std::string Message = [code = Code] { return SystemCodeToString(code); }();
	};

	class Error : std::runtime_error
	{
	public:
		Error() = default;
		Error(const std::string& message) : std::runtime_error(message) {}
		Error(
			std::string_view msg, 
			const std::source_location& loc = std::source_location::current(),
			const std::stacktrace& trace = std::stacktrace::current()
		) : std::runtime_error(Format(msg, loc, trace))
		{ }

		static auto Format(
			std::string_view message, 
			const std::source_location& loc, 
			const std::stacktrace& trace
		) -> std::string
		{
			return std::format("{} at {} of {}:{}\n{}", message, loc.function_name(), loc.line(), loc.file_name(), trace);
		}
	};

	class Win32Error : public Error
	{
	public:
		Win32Error(
			Win32::DWORD code,
			std::string_view message,
			const std::source_location& loc = std::source_location::current(),
			const std::stacktrace& trace = std::stacktrace::current()
		) : Error(Format(message, code, loc, trace), loc)
		{}
		static auto Format(
			std::string_view message,
			Win32::DWORD code,
			const std::source_location& loc,
			const std::stacktrace& trace
		) -> std::string
		{
			return std::format("{}: {} at {} of {}:{}\n{}",
				message,
				SystemCodeToString(code),
				loc.function_name(),
				loc.line(),
				loc.file_name(),
				trace
			);
		}
	};
}
