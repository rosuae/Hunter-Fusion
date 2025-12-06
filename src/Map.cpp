#include "Map.h"
#include "Player.h"
#include "Enemy.h"
#include "Portal.h"
#include "Entity.h"
#include "ResourceManager.h"
#include "GameExceptions.h"
#include <random>

namespace {
    int randomInt(const int min , const int max) {
        static std::mt19937 gen(std::random_device{}());
        return std::uniform_int_distribution(min, max)(gen);
    }
}

Map::Map(std::string n, const std::string& filePath, ResourceManager& resManager_, std::list<sf::Sound>& playingSounds_):
    MapNume{std::move(n)},
    resManager{resManager_},
    tileSprite{resManager.getTexture("tile.png")},
    tileBackgroundSprite{resManager.getTexture("backgroudtile.png")},
    playingSounds{playingSounds_} {

    std::ifstream file(filePath);
    if (!file.is_open()) {
        throw ResourceException("Couldn't read map from file: " + filePath);
    }

    std::string line;
    std::vector<sf::Vector2i> doorLocations;

    int y_ = 0;
    int currentDoorIndex = 0;
    bool readingMapLayout = true;

    while (std::getline(file, line)) {
        if (line.empty()) continue;
        std::istringstream iss(line);
        std::string firstWord;
        iss >> firstWord;

        if (firstWord == "DATA") {
            readingMapLayout = false;
            iss >> firstWord;
        }
        if (readingMapLayout) {
            if (line.front() == '#') {
                mapLayout.push_back(line);
                for (int x = 0; static_cast<size_t>(x) < line.size(); ++x) {
                    if (line[x] == 'D') {
                        doorLocations.emplace_back(x, y_);
                    }
                }
                y_++;
            }
        }
        else {
            if (firstWord == "PORTAL") {
                if (static_cast<size_t>(currentDoorIndex) >= doorLocations.size()) {
                    throw ResourceException("Warning: Defined more PORTALS in text than 'D' blocks on map.");
                }
                std::string fileName;
                float spawnX, spawnY;
                if (iss >> fileName >> spawnX >> spawnY) {
                    int autoX = doorLocations[currentDoorIndex].x;
                    int autoY = doorLocations[currentDoorIndex].y;

                    float worldX = static_cast<float>(autoX) * TILE_SIZE;
                    float worldY = static_cast<float>(autoY) * TILE_SIZE;

                    auto newPortal = std::make_unique<Portal>(
                        "Portal",
                        worldX,
                        worldY,
                        resManager.getTexture("portal.png"),
                        fileName,
                        sf::Vector2f(spawnX * TILE_SIZE, spawnY * TILE_SIZE),
                        playingSounds,
                        resManager.getSound("portalactive.wav")
                    );

                    if (autoX + 1 < static_cast<int>(mapLayout[autoY].size())) {
                        if (mapLayout[autoY][autoX + 1] == '#') {
                            newPortal->flipHorizontally();
                        }
                    }

                    solidEntitiesCache.push_back(newPortal.get());
                    entities.push_back(std::move(newPortal));
                    currentDoorIndex++;
                }
            }
            else if (firstWord == "ENEMIES") {
                std::string eName;
                int eDmg;
                if (iss >> eName >> eDmg) {
                    enemies.emplace_back(eName, eDmg);
                }
            }
        }
    }

    file.close();

    bool playerFound = false;
    for (size_t y = 0; y < mapLayout.size(); ++y)
        for (size_t x = 0; x <  mapLayout[y].size(); ++x) {
            char tileType = mapLayout[y][x];
            if (tileType == 'E') {
                enemySpawns.emplace_back(std::pair(x, y));
            } else if (tileType == 'P') {
                playerSpawn = std::pair(x, y);
                playerFound = true;
            }
        }

    if (!playerFound) {
        playerSpawn = {1, 1};
    }
}

void Map::spawnEnemies() {
    if (enemySpawns.empty() || enemies.empty()) return;

    for (const auto& enemy_ : enemies) {
        try {
            auto [x, y] = generateEnemySpawn();
            auto newEnemy = std::make_unique<Enemy>(
                enemy_.first,
                enemy_.second,
                x,
                y,
                resManager.getTexture("enemy.png"),
                playingSounds,
                resManager.getSound("enemydamage.wav"),
                resManager.getSound("enemydeath.wav")
            );
            newEnemy->setTarget(playerTarget);
            addEntity(std::move(newEnemy));
        }
        catch (const MapEntityException& e) {
            std::cout << e.what() << "Info: No enemy spawn in this room." << std::endl;
        }
    }
}

void Map::drawMap(sf::RenderWindow& window) {

    for (size_t y = 0; y < mapLayout.size(); ++y)
        for (size_t x = 0; x <  mapLayout[y].size(); ++x) {
            char tileType = mapLayout[y][x];

            if (tileType == '#') {
                tileSprite.setPosition(sf::Vector2f (static_cast<float>(x) * TILE_SIZE, static_cast<float> (y) * TILE_SIZE));
                window.draw(tileSprite);
            }else
                {
                tileBackgroundSprite.setPosition(sf::Vector2f (static_cast<float>(x) * TILE_SIZE, static_cast<float> (y) * TILE_SIZE));
                window.draw(tileBackgroundSprite);
            }
        }
}

void Map::addEntity(std::unique_ptr<Entity> entity) {
    if (!entity)
        return;

    if (isWall(entity->getBounds(), false)) {
        throw MapEntityException("Unreachable position ", entity->getPos());
    }

    if (entity->isObstacle()) {
        solidEntitiesCache.push_back(entity.get());
    }

    entities.push_back(std::move(entity));
}

void Map::updateEntities(float deltaTime) {
    for (auto& ent : entities) {
        ent->behavior(deltaTime, *this);
    }
}

void Map::drawEntities(sf::RenderWindow& window) const{
    for (const auto& ent : entities) {
        ent->draw(window);
        sf::FloatRect bounds = ent->getBounds();
        sf::RectangleShape debugRect;
        debugRect.setPosition(bounds.position);
        debugRect.setSize(bounds.size);
        debugRect.setFillColor(sf::Color::Transparent);
        debugRect.setOutlineColor(sf::Color::Red);
        debugRect.setOutlineThickness(2.0f);
        window.draw(debugRect);
    }
}

const Portal* Map::getPortalCollision(const sf::FloatRect& playerBounds) const {
    for (const auto& ent : entities) {
        if (auto portalPtr = dynamic_cast<const Portal*>(ent.get())) {
            if (portalPtr->getBounds().findIntersection(playerBounds).has_value()) {
                return portalPtr;
            }
        }
    }
    return nullptr;
}

std::pair<float, float> Map::generateEnemySpawn() const {
    if (enemySpawns.empty()) {
        throw MapEntityException("Symbol 'E' missing form map generation. No existing enemy spawns.", {-999.f, -999.f});
    }

    int randomIndex = randomInt(0, static_cast<int>(enemySpawns.size()) - 1);

    auto spawnGrid = enemySpawns[randomIndex];

    float spawnX = spawnGrid.first * TILE_SIZE;
    float spawnY = spawnGrid.second * TILE_SIZE;

    return {spawnX, spawnY};
}

std::pair<float, float> Map::getPlayerSpawn() const {
    auto spawnGrid = playerSpawn;
    float spawnX = spawnGrid.first * TILE_SIZE;
    float spawnY = spawnGrid.second * TILE_SIZE;

    return {spawnX, spawnY};
}

int Map::removeDeadEntities() {
    std::erase_if(solidEntitiesCache, [](const Entity* e) {
        return !e->isAlive();
    });

    int deadcount = 0;
    for (const auto& ent : entities)
        if (!ent->isAlive()) {
            deadcount++;
        }

    std::erase_if(entities,
        [](const std::unique_ptr<Entity>& en) {
            return !en->isAlive();
        });

    return deadcount;
}

Map::~Map() { std::cout << "S a apelat destructor Map \n";}

bool Map::isWall(const sf::FloatRect& bounds, bool checkEntities) const {
    int startX = static_cast<int> (bounds.position.x / TILE_SIZE);
    int startY = static_cast<int> (bounds.position.y / TILE_SIZE);
    int endX = static_cast<int> ((bounds.position.x + bounds.size.x) / TILE_SIZE);
    int endY = static_cast<int> ((bounds.position.y + bounds.size.y) / TILE_SIZE);

    for (int y = startY; y <= endY; ++y) {
        if (y < 0 || y >= static_cast<int>(mapLayout.size()))
            continue;
        for (int x = startX; x <=  endX; ++x) {
            if (x < 0 || x >= static_cast<int>(mapLayout[y].size()))
                continue;
            if (mapLayout[y][x] == '#') {
                sf::FloatRect tileBounds(
                    sf::Vector2f(static_cast<float>(x) * TILE_SIZE, static_cast<float>(y) * TILE_SIZE),
                    sf::Vector2f(TILE_SIZE, TILE_SIZE)
                    );
                if (tileBounds.findIntersection(bounds).has_value())
                    return true;
            }
        }
    }
    if (checkEntities) {
        for (const auto* ent : solidEntitiesCache) {
            if (ent->isObstacle()) {
                if (ent->getBounds().findIntersection(bounds).has_value()) {
                    return true;
                }
            }
        }
    }
    return false;
}

std::vector<std::unique_ptr<Entity>>& Map::getEntities() {
    return entities;
}