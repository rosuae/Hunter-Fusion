#ifndef OOP_MAP_H
#define OOP_MAP_H

#include <iostream>
#include <sstream>
#include <utility>
#include <fstream>
#include <vector>
#include <list>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>

class Player;
class Entity;
class Enemy;
class ResourceManager;

struct Portal {
    sf::FloatRect bounds;
    std::string nextMapFile;
    sf::Vector2f playerSpawnPosition;
};

class Map {
    std::string MapNume;
    ResourceManager& resManager;
    std::vector<std::unique_ptr<Entity>> entities;
    std::vector<std::string> mapLayout;
    std::vector<std::pair<float, float>> enemySpawns;
    std::vector<Portal> portals;
    std::pair<float, float> playerSpawn;
    const float TILE_SIZE = 96.f;
    sf::Sprite tileSprite;
    sf::Sprite tileBackgroundSprite;

    Player* playerTarget = nullptr;
    std::list <sf::Sound> playingSounds;
    std::vector<std::pair<std::string, int>> enemies;
public:

    Map(std::string n, const std::string& filePath, ResourceManager& resManager, std::list<sf::Sound>& playingSounds_);

    void spawnEnemies();
    void drawMap(sf::RenderWindow& window);
    void addEntity(std::unique_ptr<Entity> entity);
    void updateEntities(float deltaTime);
    void drawEntities(sf::RenderWindow& window) const;
    void setPlayerTarget(Player* p) {playerTarget = p;}
    [[nodiscard]]int removeDeadEntities();

    ~Map();

    friend std::ostream& operator<< (std::ostream& out, const Map& m) {
        out << " Nume harta: " << m.MapNume << " Dimensiune Tile: " << m.TILE_SIZE;
        return out;
    }

    const Portal* getPortalCollision(const sf::FloatRect& playerBounds) const;
    [[nodiscard]]std::pair<float, float> generateEnemySpawn () const;
    [[nodiscard]]std::pair<float, float> getPlayerSpawn() const;
    [[nodiscard]]bool isWall (const sf::FloatRect& bounds) const;
    [[nodiscard]]std::vector<std::unique_ptr<Entity>>& getEntities();
};

#endif