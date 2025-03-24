#include "include/App.hpp"
#include "include/Constants.hpp"

namespace WINDOWS
{
#include "Windows.h"
}

int WINAPI WinMain(WINDOWS::HINSTANCE hInstance, WINDOWS::HINSTANCE hPrevInstance,
    WINDOWS::PSTR lpCmdLine, int nCmdShow)
{
    App Charter(Constants::WindowWidth, Constants::WindowHeight, Constants::title);

    while (!Charter.IsClosed())
    {
        Charter.Update();
        Charter.Draw();
    }

    Charter.Destroy();

    return 0;
}