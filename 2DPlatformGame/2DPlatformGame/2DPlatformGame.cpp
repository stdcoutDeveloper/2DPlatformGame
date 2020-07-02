// 2DPlatformGame.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "Game.h"

void main(int argc, void** argv[])
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
}
