#include <iostream>

import DemoApp;
import core.com.comthreadscope;

int main(int argc, char* args[])
{
    Core::COM::COMThreadScope scope;

    DemoApp app;
    app.Initialize();

    return static_cast<int>(app.RunMessageLoop());
}
