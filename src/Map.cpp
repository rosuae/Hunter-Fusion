#include "Map.h"
#include "Player.h"
#include "Enemy.h"
#include "Entity.h"
#include "ResourceManager.h"
#include "GameExceptions.h"
#include <random>

namespace {
    int randomInt(const int min, const int max) {
        static std::mt19937 gen(std::random_device{}());
        return std::uniform_int_distribution(min, max)(gen);
    }
}

Map::Map(std::string n, const std::string& filePath, ResourceManager& resManager):
    MapNume{std::move(n)},
    tileSprite{resManager.getTexture("tile.png")},
    tileBackgroundSprite{resManager.getTexture("backgroudtile.png")} {

    std::ifstream file(filePath);
    if (!file.is_open()) {
        throw ResourceException("Couldn't read map from file: " + filePath);
    }

    std::string line;
    std::vector<sf::Vector2i> doorLocations;

    int y_ = 0;
    while (std::getline(file, line)) {
        if (line.find("DATA") != std::string::npos) break;
        if (line.empty()) continue;
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

    int currentDoorIndex = 0;
    do {
        if (line.empty()) continue;
        if (line.find("PORTAL") != std::string::npos) {
            std::istringstream iss(line);
            std::string word;
            iss >> word;
            if (word == "DATA") {
                iss >> word;
            }
            if (word == "PORTAL") {
                if (static_cast<size_t>(currentDoorIndex) >= doorLocations.size()) {
                    std::cout << "Warning: Not enough portals defined for 'D' blocks on map" << std::endl;
                    continue;
                }
                std::string fileName;
                float spawnX, spawnY;
                if (iss >> fileName >> spawnX >> spawnY) {
                    Portal newPortal;
                    int autoX = doorLocations[currentDoorIndex].x;
                    int autoY = doorLocations[currentDoorIndex].y;

                    newPortal.bounds = sf::FloatRect(sf::Vector2f(
                        static_cast<float>(autoX) * TILE_SIZE,
                        static_cast<float>(autoY) * TILE_SIZE),
                        sf::Vector2f(TILE_SIZE, TILE_SIZE)
                    );

                    float finalX = spawnX * TILE_SIZE;
                    float finalY = spawnY * TILE_SIZE;

                    newPortal.playerSpawnPosition = sf::Vector2f(finalX, finalY);
                    newPortal.nextMapFile = fileName;

                    portals.push_back(newPortal);
                    currentDoorIndex++;
                }
            }
        }
    } while (std::getline(file, line));

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
        // throw ResourceException("No playerSpawn found: " + filePath);
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

    if (isWall(entity->getBounds())) {
        const sf::Vector2f pos = entity->getPos();

        throw MapEntityException("Unreachable position ", pos);
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
    }
}

const Portal* Map::getPortalCollision(const sf::FloatRect& playerBounds) const {
    for (const auto& portal : portals) {
        if (portal.bounds.findIntersection(playerBounds).has_value()) {
            return &portal;
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

bool Map::isWall(const sf::FloatRect& bounds) const {
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
    return false;
}

std::vector<std::unique_ptr<Entity>>& Map::getEntities() {
    return entities;
}