#include "Map.h"

Map::Map(std::string n, int sizex_, int sizey_, Player& p):
    MapNume{std::move(n)},
    sizeX{sizex_},
    sizeY{sizey_},
    MyPlayer{p} {
    std::cout<<"Constructor Map\n";
}

void Map::addEnemy(const Enemy& enemy) {
    enemies.push_back(enemy);
}

Map::~Map() { std::cout << "S a apelat destructor Map \n";}

std::vector<Enemy>& Map::getEnemies() {
    return enemies;
}