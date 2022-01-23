#include <iostream>
#include <stdexcept>
#include <Windows.h>

import core.com.comthreadscope;
import core.error.win32error;
import spaceinvaders.mainwindow;
import spaceinvaders.spaceinvadersgame;

int main(int argc, char* args[]) try
{
    // https://docs.microsoft.com/en-us/windows/win32/api/heapapi/nf-heapapi-heapsetinformation
    if (!HeapSetInformation(nullptr, HeapEnableTerminationOnCorruption, nullptr, 0))
        throw Core::Error::Win32Error("HeapSetInformation() failed", GetLastError());

    Core::COM::COMThreadScope scope;

    SpaceInvaders::SpaceInvadersGame m_gameApp;
    m_gameApp.Initialise();
    return static_cast<int>(m_gameApp.RunMessageLoop());
}
catch (const std::exception& ex)
{
    std::wcerr << ex.what() << std::endl;
    return 1;
}