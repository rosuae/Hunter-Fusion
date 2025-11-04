#ifndef OOP_MAP_H
#define OOP_MAP_H

#include <iostream>
#include <fstream>
#include <vector>
#include <SFML/Graphics.hpp>

class Player;
class Enemy;
class ResourceManager;

class Map {
    std::string MapNume;
    Player& MyPlayer;
    std::vector<Enemy> enemies;
    std::vector<std::string> mapLayout;
    const float TILE_SIZE = 64.f;
    sf::Sprite tileSprite;
public:

    Map(std::string n, Player& p, const std::string& filePath, ResourceManager& resManager);

    void drawMap(sf::RenderWindow& window);

    void addEnemy(const Enemy& enemy);

    ~Map();

    friend std::ostream& operator<< (std::ostream& out, const Map& m) {
        out << " Nume harta: " << m.MapNume << " Dimensiune Tile: " << m.TILE_SIZE;
        return out;
    }

    bool isWall (const sf::FloatRect& bounds) const;

    [[nodiscard]]std::vector<Enemy>& getEnemies();
};

#endif