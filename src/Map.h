#ifndef OOP_MAP_H
#define OOP_MAP_H

#include <iostream>
#include <utility>
#include <fstream>
#include <vector>
#include <SFML/Graphics.hpp>

class Player;
class Entity;
class Enemy;
class ResourceManager;

class Map {
    std::string MapNume;
    std::vector<std::unique_ptr<Entity>> entities;
    std::vector<std::string> mapLayout;
    std::vector<std::pair<float, float>> enemySpawns;
    std::pair<float, float> playerSpawn;
    const float TILE_SIZE = 96.f;
    sf::Sprite tileSprite;
    sf::Sprite tileBackgroundSprite;
public:

    Map(std::string n, const std::string& filePath, ResourceManager& resManager);

    void drawMap(sf::RenderWindow& window);

    void addEntity(std::unique_ptr<Entity> entity);

    void updateEntities(float deltaTime);
    void drawEntities(sf::RenderWindow& window) const;


    [[nodiscard]]int removeDeadEntities();

    ~Map();

    friend std::ostream& operator<< (std::ostream& out, const Map& m) {
        out << " Nume harta: " << m.MapNume << " Dimensiune Tile: " << m.TILE_SIZE;
        return out;
    }

    [[nodiscard]]std::pair<float, float> generateEnemySpawn () const;
    [[nodiscard]]std::pair<float, float> getPlayerSpawn() const;
    [[nodiscard]]bool isWall (const sf::FloatRect& bounds) const;
    [[nodiscard]]std::vector<std::unique_ptr<Entity>>& getEntities();
};

#endif