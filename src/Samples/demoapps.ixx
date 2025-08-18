#pragma comment(lib, "d2d1.lib")
#pragma comment(lib, "Dwrite.lib")
#pragma comment(lib, "Windowscodecs.lib")

export module demoapps;
import std;
import core;
export import :rectangles;
export import :directwrite;
export import :bitmap;

// For a list of D2D error codes, see: https://docs.microsoft.com/en-us/windows/win32/direct2d/direct2d-error-codes
extern "C" auto main(int argc, char* args[]) -> int
try
{
    //Rectangles::Run();
    DirectWrite::Run();
    Bitmap::Run();
}
catch (const std::exception& ex)
{
    std::wcerr << ex.what() << std::endl;
    return 1;
}
