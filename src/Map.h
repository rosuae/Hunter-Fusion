#ifndef OOP_MAP_H
#define OOP_MAP_H

#include <sstream>
#include <utility>
#include <fstream>
#include <vector>
#include <list>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>

class Entity;
class Player;
class Portal;
class Enemy;
class ResourceManager;

class Map {
    std::string MapNume;
    ResourceManager& resManager;
    std::vector<std::unique_ptr<Entity>> entities;
    std::vector<std::string> mapLayout;
    std::vector<std::pair<float, float>> enemySpawns;
    std::pair<float, float> playerSpawn;
    static const float TILE_SIZE;
    sf::Sprite tileSprite;
    sf::Sprite tileBackgroundSprite;

    Player* playerTarget = nullptr;
    std::list <sf::Sound> playingSounds;
    std::vector<std::pair<std::string, int>> enemies;
    std::vector<Entity*> solidEntitiesCache;
public:

    Map(std::string n, const std::string& filePath, ResourceManager& resManager, std::list<sf::Sound>& playingSounds_);

    void spawnEnemies();
    void drawMap(sf::RenderWindow& window);
    void addEntity(std::unique_ptr<Entity> entity);
    void updateEntities(float deltaTime) const;
    void drawEntities(sf::RenderWindow& window) const;
    void setPlayerTarget(Player* p) {playerTarget = p;}
    [[nodiscard]]int removeDeadEntities();

    ~Map();

    friend std::ostream& operator<< (std::ostream& out, const Map& m) {
        out << " Nume harta: " << m.MapNume << " Dimensiune Tile: " << TILE_SIZE;
        return out;
    }

    const Portal* getPortalCollision(const sf::FloatRect& playerBounds) const;
    [[nodiscard]]static sf::Vector2f gridToWorld(int x, int y);
    [[nodiscard]]std::pair<float, float> generateEnemySpawn () const;
    [[nodiscard]]std::pair<float, float> getPlayerSpawn() const;
    [[nodiscard]]bool isWall (const sf::FloatRect& bounds, bool checkEntities = false) const;
    [[nodiscard]]std::vector<std::unique_ptr<Entity>>& getEntities();
};

#endif