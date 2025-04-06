#include <SFML/Graphics.hpp>

#include "GameEngine.h"

int main()
{
    GameEngine gameEngine("./config/");
    gameEngine.run();
    return 0;
}