// 2DPlatformGame.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "Game.h"

int main(int argc, const char* argv[])
{
    // Program entry point.
    {
        Game game;
        while (!game.GetWindow()->IsDone())
        {
            game.Update();
            game.Render();
            game.LateUpdate();
        }
    }
    system("PAUSE");

    return EXIT_SUCCESS;
}
