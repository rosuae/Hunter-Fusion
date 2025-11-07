#include "src/Game.h"
#include "src/ResourceManager.h"
#include <iostream>

int main() {

    ResourceManager& resManager = ResourceManager::Instance();

    try {
        Game game;

        game.run();
    }
    catch (const std::exception& e) {
        std::cerr << "A apărut o eroare fatală: " << e.what() << std::endl;
        resManager.cleanup();
        return 1;
    }
    catch (...) {
        std::cerr << "A apărut o eroare necunoscută!" << std::endl;
        resManager.cleanup();
        return 2;
    }

    resManager.cleanup();

    return 0;
}