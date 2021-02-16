// Adapted from the offical sample with an additional fix for a deprecation warning:
// https://docs.microsoft.com/en-us/windows/win32/direct2d/direct2d-quickstart

#include "DemoApp.hpp"
#include <iostream>

#pragma comment(lib, "d2d1.lib")

/* Use this signature for main when using SUBSYSTEM:WINDOWS
int WINAPI WinMain(
    HINSTANCE hInstance,
    HINSTANCE hPrevInstance,
    LPSTR lpCmdLine,
    int nCmdShow
)
*/
int main(int argc, char* args[])
{
    // Use HeapSetInformation to specify that the process should
    // terminate if the heap manager detects an error in any heap used
    // by the process.
    // The return value is ignored, because we want to continue running in the
    // unlikely event that HeapSetInformation fails.
    HeapSetInformation(NULL, HeapEnableTerminationOnCorruption, NULL, 0);

    if (SUCCEEDED(CoInitialize(NULL)))
    {
        {
            DemoApp app;

            if (SUCCEEDED(app.Initialize()))
            {
                app.RunMessageLoop();
            }
        }
        CoUninitialize();
    }

    return 0;
}
