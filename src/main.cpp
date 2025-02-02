#include "include/App.hpp"
#include "include/Constants.hpp"

int main()
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