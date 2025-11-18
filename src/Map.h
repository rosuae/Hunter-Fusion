#ifndef OOP_MAP_H
#define OOP_MAP_H

#include <iostream>
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
    const float TILE_SIZE = 64.f;
    sf::Sprite tileSprite;
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

    [[nodiscard]]bool isWall (const sf::FloatRect& bounds) const;

    [[nodiscard]]std::vector<std::unique_ptr<Entity>>& getEntities();
};

#endif