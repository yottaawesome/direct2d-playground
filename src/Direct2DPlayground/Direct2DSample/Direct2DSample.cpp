// Adapted from the offical sample with an additional fix for a deprecation warning:
// https://docs.microsoft.com/en-us/windows/win32/direct2d/direct2d-quickstart

#include "DemoApp.hpp"
#include <iostream>

#pragma comment(lib, "d2d1.lib")

int main(int argc, char* args[])
{
    DemoApp demoApp;
    demoApp.Initialize();
    demoApp.RunMessageLoop();

    return 0;
}
