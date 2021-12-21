// Adapted from the offical sample with an additional fix for a deprecation warning:
// https://docs.microsoft.com/en-us/windows/win32/direct2d/direct2d-quickstart
// See also: https://docs.microsoft.com/en-us/windows/win32/direct2d/getting-started-with-direct2d
// and https://docs.microsoft.com/en-us/windows/win32/direct2d/direct2d-portal

#include <iostream>
#include <stdexcept>
#include <Windows.h>

#pragma comment(lib, "d2d1.lib")

/* Use this signature for main when using SUBSYSTEM:WINDOWS
int WINAPI WinMain(
    HINSTANCE hInstance,
    HINSTANCE hPrevInstance,
    LPSTR lpCmdLine,
    int nCmdShow
)
*/

import DemoApp;

int main(int argc, char* args[]) try
{
    // Use HeapSetInformation to specify that the process should
    // terminate if the heap manager detects an error in any heap used
    // by the process.
    // The return value is ignored, because we want to continue running in the
    // unlikely event that HeapSetInformation fails.
    HeapSetInformation(nullptr, HeapEnableTerminationOnCorruption, nullptr, 0);
    if (FAILED(CoInitialize(nullptr)))
        throw std::runtime_error("CoInitialize() failed");

    {
        DemoApp app;
        if (SUCCEEDED(app.Initialize()))
            app.RunMessageLoop();
    }
    CoUninitialize();

    return 0;
}
catch (const std::exception& ex)
{
    std::wcerr << ex.what() << std::endl;
}