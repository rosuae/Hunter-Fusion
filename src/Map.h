#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <Player.h>
#include <Enemy.h>

class Map {
    std::string MapNume;
    int sizeX, sizeY;
    Player MyPlayer;
    std::vector<Enemy> enemies;

public:
    Map(const std::string& n, int sizex_, int sizey_, const Player& p);

protected:
    void addEnemy(const Enemy& enemy);
};