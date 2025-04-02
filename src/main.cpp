/*
#################################################################################
                                    main.cpp

This is a very barebones implementation of the main loop no fancy delta time or 
other complexities, since they aren't needed here. But it has that fucking
ugly-ass preprocessor #if statement, which is why I hate this file.
#################################################################################
*/

#include "include/App.hpp"
#include "include/Constants.hpp"

#ifdef IS_RELEASE_BUILD //Start this only when we compile in Release
    namespace WINDOWS //Hack because Raylib uses the same functions that Windows does
    {
        #include "Windows.h"
    }

int WINAPI WinMain(WINDOWS::HINSTANCE hInstance, WINDOWS::HINSTANCE hPrevInstance,
        WINDOWS::PSTR lpCmdLine, int nCmdShow)
#else //Or start main idk
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