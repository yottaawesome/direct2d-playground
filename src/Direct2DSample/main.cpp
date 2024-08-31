// Adapted from the offical sample with an additional fix for a deprecation warning:
// https://docs.microsoft.com/en-us/windows/win32/direct2d/direct2d-quickstart
// See also: https://docs.microsoft.com/en-us/windows/win32/direct2d/getting-started-with-direct2d
// and https://docs.microsoft.com/en-us/windows/win32/direct2d/direct2d-portal

#include <iostream>
#include <stdexcept>
#include <Windows.h>
#include <wincodec.h>
#include <wrl/client.h>

//#pragma comment(lib, "d2d1.lib")

/* Use this signature for main when using SUBSYSTEM:WINDOWS
int WINAPI WinMain(
    HINSTANCE hInstance,
    HINSTANCE hPrevInstance,
    LPSTR lpCmdLine,
    int nCmdShow
)
*/

import DemoApp;
import core.com.comthreadscope;
import core.error.win32error;
import core.error.comerror;
import core.wic.wicimagingfactory;

// For a list of D2D error codes, see: https://docs.microsoft.com/en-us/windows/win32/direct2d/direct2d-error-codes
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