#include "src/Game.h"
#include "src/ResourceManager.h"
#include "src/GameExceptions.h"
#include <iostream>

int main() {

    ResourceManager& resManager = ResourceManager::Instance();

    try {
        Game game;
        game.run();
    }
    catch (const ResourceException& e) {
        std::cout << e.what() << std::endl;
        resManager.cleanup();
        return 1;
    }

    catch (const MapEntityException& e) {
        std::cout << e.what() << std::endl;
        resManager.cleanup();
        return 2;
    }

    resManager.cleanup();

    return 0;
}