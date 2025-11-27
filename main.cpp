#include "src/Game.h"
#include "src/GameExceptions.h"
#include <iostream>
#include <thread>

using namespace std::chrono_literals;

int main() {

    try {
        Game game;
        game.run();
    }
    catch (const ResourceException& e) {
        std::cout << e.what() << std::endl;
        return 1;
    }
    catch (const MapEntityException& e) {
        std::cout << e.what() << std::endl;
        return 2;
    }

    return 0;
}