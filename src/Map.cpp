#include "Map.h"
#include "Game.h"
#include "Player.h"
#include "Enemy.h"
#include "Portal.h"
#include "Entity.h"
#include "ResourceManager.h"
#include "GameExceptions.h"
#include "EnemyFactory.h"
#include <random>

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
                int spawnX, spawnY;
                if (iss >> fileName >> spawnX >> spawnY) {
                    int autoX = doorLocations[currentDoorIndex].x;
                    int autoY = doorLocations[currentDoorIndex].y;

                    sf::Vector2f world = gridToWorld(autoX, autoY);

                    auto newPortal = std::make_unique<Portal>(
                        "Portal",
                        world.x,
                        world.y,
                        resManager.getTexture("portal.png"),
                        fileName,
                        gridToWorld(spawnX, spawnY),
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
    if (enemySpawns.empty() || enemies.empty() || !playerTarget) return;

    for (const auto& enemy_ : enemies) {
        try {
            auto [x, y] = generateEnemySpawn();
            std::unique_ptr newEnemy = EnemyFactory::createEnemy(
                enemy_.first,
                x,
                y,
                resManager,
                playingSounds,
                playerTarget
            );
            if (newEnemy) {
                addEntity(std::move(newEnemy));
            }
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
                tileSprite.setPosition(gridToWorld(static_cast<int>(x), static_cast<int>(y)));
                window.draw(tileSprite);
            }else
                {
                tileBackgroundSprite.setPosition(gridToWorld(static_cast<int>(x), static_cast<int>(y)));
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

    const bool isObs = entity->isObstacle();
    entities.push_back(std::move(entity));

    if (isObs) {
        solidEntitiesCache.push_back(entities.back().get());
    }
}

void Map::updateEntities(float deltaTime) const {
    for (const auto& ent : entities) {
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
        if (const auto portalPtr = dynamic_cast<const Portal*>(ent.get())) {
            if (portalPtr->getBounds().findIntersection(playerBounds).has_value()) {
                return portalPtr;
            }
        }
    }
    return nullptr;
}

const float Map::TILE_SIZE = 96.0f;

sf::Vector2f Map::gridToWorld(int x, int y) {
    return {static_cast<float>(x) * TILE_SIZE,
                        static_cast<float>(y) * TILE_SIZE};
}

std::pair<float, float> Map::generateEnemySpawn() const {
    if (enemySpawns.empty()) {
        throw MapEntityException("Symbol 'E' missing form map generation. No existing enemy spawns.", {-999.f, -999.f});
    }

    int randomIndex = Game::generateRandomInt(0, static_cast<int>(enemySpawns.size()) - 1);

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
    int deadCount = 0;
    std::erase_if(entities, [&](const std::unique_ptr<Entity>& en) {
        if (!en->isAlive()) {
            deadCount++;
            return true;
        }
        return false;
    });

    solidEntitiesCache.clear();
    for (const auto& ent : entities) {
        if (ent && ent->isObstacle()) {
            solidEntitiesCache.push_back(ent.get());
        }
    }
    return deadCount;
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
            if (ent == nullptr) continue;
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