module;

#include <Windows.h>

export module core:error_functions;
import std;

export namespace Core::Error
{
    template<typename S>
    struct ErrorFormatter
    {
        static S FormatCode(const DWORD errorCode, const DWORD flags, HMODULE moduleToSearch) { static_assert(false); }
    };

    template<>
    struct ErrorFormatter<std::string>
    {
        static std::string FormatCode(const DWORD errorCode, const DWORD flags, HMODULE moduleToSearch)
        {
            void* messageBuffer = nullptr;
            FormatMessageA(
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
                    GetLastError()
                );

            std::string msg(static_cast<char*>(messageBuffer));
            if (LocalFree(messageBuffer))
                std::wcerr << std::format(L"{}: LocalFree() failed: {}\n", TEXT(__FUNCSIG__), GetLastError());

            return msg;
        }
    };

    template<>
    struct ErrorFormatter<std::wstring>
    {
        static std::wstring FormatCode(const DWORD errorCode, const DWORD flags, HMODULE moduleToSearch)
        {
            void* messageBuffer = nullptr;
            FormatMessageW(
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
                    GetLastError()
                );

            std::wstring msg(static_cast<wchar_t*>(messageBuffer));
            if (LocalFree(messageBuffer))
                std::wcerr << std::format(L"{}: LocalFree() failed: {}\n", TEXT(__FUNCSIG__), GetLastError());

            return msg;
        }
    };

    // Translates Win32 errors, including COM errors, to human-readable error strings.
    // Some error codes are defined in specific modules; pass in the module as the 
    // second parameter for the function to translate such error codes.
    template<typename STR_T>
    STR_T TranslateErrorCode(const DWORD errorCode, const std::wstring& moduleName)
        requires std::is_same<std::string, STR_T>::value || std::is_same<std::wstring, STR_T>::value
    {
        // Retrieve the system error message for the last-error code
        HMODULE moduleHandle = moduleName.empty() ? nullptr : LoadLibraryW(moduleName.c_str());
        DWORD flags =
            FORMAT_MESSAGE_ALLOCATE_BUFFER |
            FORMAT_MESSAGE_FROM_SYSTEM |
            FORMAT_MESSAGE_IGNORE_INSERTS |
            (moduleHandle ? FORMAT_MESSAGE_FROM_HMODULE : 0);
        STR_T errorString = ErrorFormatter<STR_T>::FormatCode(errorCode, flags, moduleHandle);
        if (moduleHandle)
            FreeLibrary(moduleHandle);

        return errorString;
    }

    // Translates a default Win32 or COM error code.
    template<typename STR_T>
    STR_T TranslateErrorCode(const DWORD errorCode)
    {
        return TranslateErrorCode<STR_T>(errorCode, L"");
    }
}