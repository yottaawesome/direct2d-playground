export module shared:ui.functions;
import std;
import :win32;

export namespace Shared
{
	void ErrorMessageBox(const std::string& message, const std::string& title = "Error")
	{
		Win32::MessageBoxA(nullptr, message.c_str(), title.c_str(), Win32::MessageBoxOptions::Ok | Win32::MessageBoxOptions::Critical);
	}

	void ErrorMessageBox(const std::wstring& message, const std::wstring& title = L"Error")
	{
		Win32::MessageBoxW(nullptr, message.c_str(), title.c_str(), Win32::MessageBoxOptions::Ok | Win32::MessageBoxOptions::Critical);
	}
}
