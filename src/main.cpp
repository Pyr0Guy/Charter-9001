#include "include/App.hpp"
#include "include/Constants.hpp"

#ifdef IS_RELEASE_BUILD
    namespace WINDOWS
    {
        #include "Windows.h"
    }

int WINAPI WinMain(WINDOWS::HINSTANCE hInstance, WINDOWS::HINSTANCE hPrevInstance,
        WINDOWS::PSTR lpCmdLine, int nCmdShow)
#else
    void main()
#endif
{
    App Charter(Constants::WindowWidth, Constants::WindowHeight, Constants::title);

    while (!Charter.IsClosed())
    {
        Charter.Update();
        Charter.Draw();
    }

    Charter.Destroy();
}