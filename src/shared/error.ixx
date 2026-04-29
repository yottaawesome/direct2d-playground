export module shared:error;
import std;
import :win32;

export namespace Shared
{
	auto SystemCodeToString(Win32::DWORD code) -> std::string
	{
		void* buffer = nullptr;
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
		if (chars == 0 or not buffer)
			return std::format("Unknown error code: {}", code);

		auto message = std::string(static_cast<char*>(buffer), chars);
		Win32::LocalFree(buffer);
		while (not message.empty() and (message.back() == '\r' or message.back() == '\n'))
			message.pop_back();
		return message;
	}

	struct SystemCode
	{
		Win32::DWORD Code = 0;
		std::string Message = [code = Code] { return SystemCodeToString(code); }();
	};

	// FormatMessage with FROM_SYSTEM resolves most HRESULTs directly, but for
	// FACILITY_WIN32-wrapped HRESULTs (HRESULT_FROM_WIN32) we get a cleaner
	// message by unwrapping to the underlying Win32 code first. Inlined here
	// because HRESULT_FACILITY / HRESULT_CODE / FACILITY_WIN32 are macros and
	// can't be re-exported through the :win32 module partition.
	auto HResultToString(Win32::HRESULT hr) -> std::string
	{
		constexpr long FacilityWin32 = 7;
		const auto facility = (static_cast<unsigned long>(hr) >> 16) & 0x1FFFul;
		if (facility == FacilityWin32)
			return SystemCodeToString(static_cast<Win32::DWORD>(hr) & 0xFFFFul);
		return SystemCodeToString(static_cast<Win32::DWORD>(hr));
	}

	struct HResult
	{
		Win32::HRESULT Code = 0;
		constexpr auto Succeeded() const noexcept { return Win32::Succeeded(Code); }
		constexpr auto Failed() const noexcept { return Win32::Failed(Code); }
		constexpr operator Win32::HRESULT() const noexcept { return Code; }
		constexpr bool operator==(const HResult& other) const noexcept = default;
		constexpr operator bool() const noexcept { return Succeeded(); }
	};

	class Error : public std::runtime_error
	{
	public:
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
		) : Error(FormatPrefix(message, code), loc, trace)
		{}

	private:
		static auto FormatPrefix(std::string_view message, Win32::DWORD code) -> std::string
		{
			return std::format("{}: {}", message, SystemCodeToString(code));
		}
	};

	class ComError : public Error
	{
	public:
		ComError(
			Win32::HRESULT hr,
			std::string_view message,
			const std::source_location& loc = std::source_location::current(),
			const std::stacktrace& trace = std::stacktrace::current()
		) : Error(FormatPrefix(message, hr), loc, trace)
		{}

	private:
		static auto FormatPrefix(std::string_view message, Win32::HRESULT hr) -> std::string
		{
			return std::format("{}: {} (0x{:08X})", message, HResultToString(hr), static_cast<unsigned long>(hr));
		}
	};
}
