#include <iostream>
#include <stdexcept>
#include <Windows.h>
#include <wincodec.h>
#include <wrl/client.h>
#pragma comment(lib, "d2d1.lib")

import DemoApp;
import core.com.comthreadscope;
import core.error.win32error;
import core.error.comerror;
import core.wic.wicimagingfactory;

int main(int argc, char* args[]) try
{
    // https://docs.microsoft.com/en-us/windows/win32/api/heapapi/nf-heapapi-heapsetinformation
    if (!HeapSetInformation(nullptr, HeapEnableTerminationOnCorruption, nullptr, 0))
        throw Core::Error::Win32Error("HeapSetInformation() failed", GetLastError());

    Core::COM::COMThreadScope scope;

    DemoApp app;
    app.Initialize();
    return static_cast<int>(app.RunMessageLoop());
}
catch (const std::exception& ex)
{
    std::wcerr << ex.what() << std::endl;
    return 1;
}

