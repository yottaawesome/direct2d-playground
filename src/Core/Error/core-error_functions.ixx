export module core:error_functions;
import std;
import :win32;

export namespace Error
{
    template<typename S>
    struct ErrorFormatter
    {
        static S FormatCode(const Win32::DWORD errorCode, const Win32::DWORD flags, Win32::HMODULE moduleToSearch) { static_assert(false); }
    };

    template<>
    struct ErrorFormatter<std::string>
    {
        static std::string FormatCode(const Win32::DWORD errorCode, const Win32::DWORD flags, Win32::HMODULE moduleToSearch)
        {
            void* messageBuffer = nullptr;
            Win32::FormatMessageA(
                flags,
                moduleToSearch,
                errorCode,
                0,
                reinterpret_cast<char*>(&messageBuffer),
                0,
                nullptr
            );
            if (!messageBuffer)
                return std::format(
                    "FormatMessageA() failed on code {} with error {}",
                    errorCode,
                    Win32::GetLastError()
                );

            std::string msg(static_cast<char*>(messageBuffer));
            if (Win32::LocalFree(messageBuffer))
                std::wcerr << std::format(L"LocalFree() failed: {}\n", Win32::GetLastError());

            return msg;
        }
    };

    template<>
    struct ErrorFormatter<std::wstring>
    {
        static std::wstring FormatCode(const DWORD errorCode, const DWORD flags, HMODULE moduleToSearch)
        {
            void* messageBuffer = nullptr;
            Win32::FormatMessageW(
                flags,
                moduleToSearch,
                errorCode,
                0,
                reinterpret_cast<wchar_t*>(&messageBuffer),
                0,
                nullptr
            );
            if (!messageBuffer)
                return std::format(
                    L"FormatMessageA() failed on code {} with error {}",
                    errorCode,
                    Win32::GetLastError()
                );

            std::wstring msg(static_cast<wchar_t*>(messageBuffer));
            if (Win32::LocalFree(messageBuffer))
                std::wcerr << std::format(L"LocalFree() failed: {}\n", Win32::GetLastError());

            return msg;
        }
    };

    // Translates Win32 errors, including COM errors, to human-readable error strings.
    // Some error codes are defined in specific modules; pass in the module as the 
    // second parameter for the function to translate such error codes.
    template<typename STR_T>
    STR_T TranslateErrorCode(const Win32::DWORD errorCode, const std::wstring& moduleName)
        requires std::is_same<std::string, STR_T>::value || std::is_same<std::wstring, STR_T>::value
    {
        // Retrieve the system error message for the last-error code
        Win32::HMODULE moduleHandle = moduleName.empty() ? nullptr : Win32::LoadLibraryW(moduleName.c_str());
        Win32::DWORD flags =
            Win32::FormatMessageFlags::AllocateBuffer |
            Win32::FormatMessageFlags::FromSystem |
            Win32::FormatMessageFlags::IgnoreInserts |
            (moduleHandle ? Win32::FormatMessageFlags::FromHModule : 0);
        STR_T errorString = ErrorFormatter<STR_T>::FormatCode(errorCode, flags, moduleHandle);
        if (moduleHandle)
            Win32::FreeLibrary(moduleHandle);

        return errorString;
    }

    // Translates a default Win32 or COM error code.
    template<typename STR_T>
    STR_T TranslateErrorCode(const Win32::DWORD errorCode)
    {
        return TranslateErrorCode<STR_T>(errorCode, L"");
    }
}