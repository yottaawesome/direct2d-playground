#include <iostream>
#include <stdexcept>
#include <Windows.h>

import MainWindow;
import core.com.comthreadscope;
import core.error.win32error;

int main(int argc, char* args[]) try
{
    // https://docs.microsoft.com/en-us/windows/win32/api/heapapi/nf-heapapi-heapsetinformation
    if (!HeapSetInformation(nullptr, HeapEnableTerminationOnCorruption, nullptr, 0))
        throw Core::Error::Win32Error("HeapSetInformation() failed", GetLastError());

    Core::COM::COMThreadScope scope;

    MainWindow app;
    app.Initialize();
    return static_cast<int>(app.RunMessageLoop());
}
catch (const std::exception& ex)
{
    std::wcerr << ex.what() << std::endl;
    return 1;
}