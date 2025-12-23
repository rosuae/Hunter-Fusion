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
class Pet;
class Portal;
class Enemy;
class Projectile;
class ResourceManager;

class Map {
    std::string MapNume;
    ResourceManager& resManager;
    std::vector<std::unique_ptr<Entity>> entities;

    std::vector<std::string> mapLayout;
    sf::VertexArray m_wallVertices;
    sf::VertexArray m_bgVertices;

    std::vector<std::pair<float, float>> enemySpawns;
    std::pair<float, float> playerSpawn;
    std::pair<float, float> petSpawn;
    bool hasPetSpawn = false;
    static const float TILE_SIZE;
    sf::Sprite tileSprite;
    sf::Sprite tileBackgroundSprite;

    Player* playerTarget = nullptr;
    std::list <sf::Sound> playingSounds;
    std::vector<std::pair<std::string, int>> enemies;
    std::vector<Entity*> solidEntitiesCache;

    int maxEnemiesAllowed = 100; // for the moment
    bool hasSpawnedEnemies = false;

    bool isWallAt(int x, int y) const;
    sf::IntRect getWallTextureRect(int x, int y) const;
    void generateMapGeometry();
    void spawnEnemies();
    std::pair<float, float> findSpawnLocation(char symbol) const;
    [[nodiscard]] std::pair<float, float> generateRandomEnemySpawn() const;
    [[nodiscard]] static sf::Vector2f gridToWorld(int x, int y);
    void handleEnemyRespawn(int enemiesDied);
public:

    Map(std::string n, const std::string& filePath, ResourceManager& resManager,
        std::list<sf::Sound>& playingSounds_);

    static float getTileSize() { return TILE_SIZE; }
    void initializeWithPlayer(Player& player);
    void initializeWithExistingPlayer(Player& player);
    void drawMap(sf::RenderWindow& window) const;
    void drawEntities(sf::RenderWindow& window) const;
    void updateEntities(float deltaTime) const;
    void cleanupAndRespawn();

    int processEnemyAttacks() const;

    void processProjectileCollisions() const;
    void spawnAdditionalEnemies(int count);
    void spawnEntityAt(std::unique_ptr<Entity> entity);

    std::unique_ptr<Pet> extractPet(const sf::FloatRect& playerBounds);
    // void depositPet(std::unique_ptr<Pet> pet);

    const std::vector<std::string>& getLayout() const { return mapLayout; }
    std::optional<std::pair<std::string, sf::Vector2f>> tryTeleport(const sf::FloatRect& playerBounds) const;

    void placeEntity(Entity& entity, char mapSymbol) const;
    [[nodiscard]]bool isWall (const sf::FloatRect& bounds, bool checkEntities = false) const;

    ~Map();

    friend std::ostream& operator<< (std::ostream& out, const Map& m) {
        out << " Nume harta: " << m.MapNume << " Dimensiune Tile: " << TILE_SIZE;
        return out;
    }
};

#endif