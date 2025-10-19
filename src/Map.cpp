#include <Map.h>

Map::Map(const std::string& n, int sizex_, int sizey_, const Player& p):
    MapNume{n}, sizeX{sizex_}, sizeY{sizey_}, MyPlayer{p}{}

void Map::addEnemy(const Enemy& enemy) {
    enemies.push_back(enemy);
}