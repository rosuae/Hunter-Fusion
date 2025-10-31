#ifndef OOP_MAP_H
#define OOP_MAP_H

#include "Player.h"
#include "Enemy.h"

class Map {
    std::string MapNume;
    int sizeX, sizeY;
    Player& MyPlayer;
    std::vector<Enemy> enemies;

public:

    Map(std::string n, int sizex_, int sizey_, Player& p);

    void addEnemy(const Enemy& enemy);

    ~Map();

    friend std::ostream& operator<< (std::ostream& out, const Map& m) {
        out << " Nume harta: " << m.MapNume << " Latime harta: " << m.sizeX << " Lungime harta: " << m.sizeY << " " << m.MyPlayer;
        out << "\n Enemies: \n";
        for (const auto& e : m.enemies) {
            out << e << "\n";
        }
        return out;
    }

    [[nodiscard]]std::vector<Enemy>& getEnemies();
};

#endif