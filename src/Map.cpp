#include "Map.h"
#include "Game.h"
#include "Player.h"
#include "Pet.h"
#include "Enemy.h"
#include "Portal.h"
#include "Pickup.h"
#include "Entity.h"
#include "ResourceManager.h"
#include "GameExceptions.h"
#include "EnemyFactory.h"
#include <random>

Map::Map(std::string n, const std::string& filePath, ResourceManager& resM, std::list<sf::Sound>& playingSounds_):
    MapNume{std::move(n)},
    resManager{resM},
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
                std::string fileName; int spawnX, spawnY;
                if (iss >> fileName >> spawnX >> spawnY) {
                    int autoX = doorLocations[currentDoorIndex].x;
                    int autoY = doorLocations[currentDoorIndex].y;
                    bool facingRight = true;

                    if (autoX + 1 < static_cast<int>(mapLayout[autoY].size())) {
                        if (mapLayout[autoY][autoX + 1] == '#')
                            facingRight = false;
                    }

                    sf::Vector2f world = gridToWorld(autoX, autoY);

                    auto newPortal = std::make_unique<Portal>(
                        "Portal",
                        world.x,
                        world.y,
                        resManager.getTexture("portal.png"),
                        fileName,
                        facingRight,
                        gridToWorld(spawnX, spawnY),
                        playingSounds,
                        resManager.getSound("portalactive.wav")
                    );

                    solidEntitiesCache.push_back(newPortal.get());
                    entities.push_back(std::move(newPortal));
                    currentDoorIndex++;
                }
            }
            else if (firstWord == "ENEMIES") {
                std::string eName; int eDmg;
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
            if (char tileType = mapLayout[y][x]; tileType == 'E') {
                enemySpawns.emplace_back(std::pair(x, y));
            } else if (tileType == 'P') {
                playerSpawn = std::pair(x, y);
                playerFound = true;
            } else if (tileType == 'C') {
                petSpawn = std::pair(x, y);
                hasPetSpawn = true;
            }
        }

    if (!playerFound) {
        playerSpawn = {1, 1};
    }

    generateMapGeometry();
}

bool Map::hasPet() const {
    for (const auto& entity : entities) {
        if (dynamic_cast<const Pet*>(entity.get())) {
            return true;
        }
    }
    return false;
}

bool Map::isWallAt(const int x, const int y) const {
    if (y < 0 || y >= static_cast<int>(mapLayout.size())) return false;
    if (x < 0 || x >= static_cast<int>(mapLayout[y].size())) return false;
    return mapLayout[y][x] == '#';
}

sf::IntRect Map::getWallTextureRect(const int x, const int y) const {
    int mask = 0;
    if (isWallAt(x, y - 1)) mask += 1;
    if (isWallAt(x - 1, y)) mask += 2;
    if (isWallAt(x + 1, y)) mask += 4;
    if (isWallAt(x, y + 1)) mask += 8;

    static const sf::Vector2i textureCoords[16] = { // for now 3x3 tileset, see 'Map creation logic' from README.md (Resources)
        {3, 3},
        {1, 2},{2, 1},{2, 2},
        {0, 1},{0, 2},{1, 1},
        {1, 2},{1, 0},{1, 0},
        {2, 0},{2, 1},{0, 0},
        {0, 1},{1, 0},{1, 1}
    };

    const int col = textureCoords[mask].x;
    const int row = textureCoords[mask].y;

    const int size = static_cast<int>(TILE_SIZE);

    return {{col * size, row * size}, {size, size}};
}

void Map::generateMapGeometry() {
    m_wallVertices.setPrimitiveType(sf::PrimitiveType::Triangles);
    m_bgVertices.setPrimitiveType(sf::PrimitiveType::Triangles);

    m_wallVertices.clear();
    m_bgVertices.clear();

    for (size_t y = 0; y < mapLayout.size(); ++y) {
        for (size_t x = 0; x < mapLayout[y].size(); ++x) {

            const auto fX = static_cast<float>(x);
            const auto fY = static_cast<float>(y);
            auto fSize = static_cast<float>(TILE_SIZE);

            const float posX = fX * fSize;
            const float posY = fY * fSize;

            sf::Vertex tl{ sf::Vector2f(posX, posY), sf::Color::White };
            sf::Vertex tr{ sf::Vector2f(posX + fSize, posY), sf::Color::White };
            sf::Vertex br{ sf::Vector2f(posX + fSize, posY + fSize), sf::Color::White };
            sf::Vertex bl{ sf::Vector2f(posX, posY + fSize), sf::Color::White };

            if (mapLayout[y][x] == '#') {
                const sf::IntRect uv = getWallTextureRect(static_cast<int>(x), static_cast<int>(y));

                auto u = static_cast<float>(uv.position.x);
                auto v = static_cast<float>(uv.position.y);

                tl.texCoords = {u, v};
                tr.texCoords = {u + fSize, v};
                br.texCoords = {u + fSize, v + fSize};
                bl.texCoords = {u, v + fSize};

                m_wallVertices.append(tl);
                m_wallVertices.append(tr);
                m_wallVertices.append(bl);
                m_wallVertices.append(tr);
                m_wallVertices.append(br);
                m_wallVertices.append(bl);
            }
            else {
                tl.texCoords = {0.f, 0.f};
                tr.texCoords = {fSize, 0.f};
                br.texCoords = {fSize, fSize};
                bl.texCoords = {0.f, fSize};

                m_bgVertices.append(tl); m_bgVertices.append(tr); m_bgVertices.append(bl);
                m_bgVertices.append(tr); m_bgVertices.append(br); m_bgVertices.append(bl);
            }
        }
    }
}

void Map::spawnEnemies() {
    if (enemySpawns.empty() || enemies.empty() || !playerTarget) return;
    for (const auto&[enemyX, enemyY] : enemySpawns) {
        try {
            const float x = enemyX * TILE_SIZE;
            const float y = enemyY * TILE_SIZE;

            int enemyIndex = 0;
            if (enemies.size() > 1) {
                enemyIndex = Game::generateRandomInt(0, static_cast<int>(enemies.size()) - 1);
            }
            const auto&[fst, snd] = enemies[enemyIndex];
            std::unique_ptr newEnemy = EnemyFactory::createEnemy(
                fst,
                x,
                y,
                resManager,
                playingSounds,
                playerTarget
            );
            if (newEnemy) {
                spawnEntityAt(std::move(newEnemy));
            }
        }
        catch (const MapEntityException& e) {
            std::cout << e.what() << " Info: Failed to spawn enemy at tile ("
                      << enemyX << ", " << enemyY << ")." << std::endl;
        }
    }
}

void Map::drawMap(sf::RenderWindow& window) const {
    window.draw(m_bgVertices, &tileBackgroundSprite.getTexture());
    window.draw(m_wallVertices, &tileSprite.getTexture());
}

void Map::processProjectileCollisions() const {
    if (!playerTarget) return;

    playerTarget->checkProjectileCollisions(entities);
}

void Map::handlePickupCollisions(Player& player) const {
    if (!player.isAlive()) return;

    const sf::FloatRect playerBounds = player.getBounds();
    for (auto& entity : entities) {
        if (entity->isAlive()) {
            if (auto* pickup = dynamic_cast<Pickup*>(entity.get())) {
                if (pickup->getBounds().findIntersection(playerBounds).has_value()) {
                    pickup->apply(player);
                }
            }
        }
    }
}

void Map::spawnAdditionalEnemies(const int count) {
    if (!playerTarget) return;

    for (int i = 0; i < count; ++i) {
        try {
            int enemyIndex = 0;
            if (enemies.size() > 1) {
                enemyIndex = Game::generateRandomInt(0, static_cast<int>(enemies.size()) - 1);
            }
            std::string enemyName = enemies[enemyIndex].first;
            auto [x, y] = generateRandomEnemySpawn();
            std::unique_ptr newEnemy = EnemyFactory::createEnemy(
                enemyName,
                x, y,
                resManager,
                playingSounds,
                playerTarget
            );
            if (newEnemy) {
                spawnEntityAt(std::move(newEnemy));
            }
        }
        catch (const MapEntityException& e) {
            std::cout << e.what() << std::endl;
        }
    }
}

void Map::spawnEntityAt(std::unique_ptr<Entity> entity) {
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

std::unique_ptr<Pet> Map::extractPet(const sf::FloatRect& playerBounds) {
    const auto it = std::find_if(entities.begin(), entities.end(),
        [&](const std::unique_ptr<Entity>& e) {
            const auto* p = dynamic_cast<Pet*>(e.get());
            return p != nullptr && p->getBounds().findIntersection(playerBounds).has_value();
        });

    if (it != entities.end()) {
        std::unique_ptr<Entity> foundEntity = std::move(*it);
        entities.erase(it);
        return std::unique_ptr<Pet>(dynamic_cast<Pet*>(foundEntity.release()));
    }
    return nullptr;
}

// void Map::depositPet(std::unique_ptr<Pet> pet) {
//     if (pet) {
//         spawnEntityAt(std::move(pet));
//     }
// }

int Map::processEnemyAttacks() const {
    int totalDamage = 0;

    for (const auto& entity : entities) {
        if (const auto enemyPtr = dynamic_cast<Enemy*>(entity.get())) {
            if (enemyPtr->isAlive()) {
                totalDamage += enemyPtr->attackPlayer();
            }
        }
    }

    return totalDamage;
}

void Map::updateEntities(const float deltaTime) const {
    for (const auto& ent : entities) {
        ent->behavior(deltaTime, *this);
    }
}

void Map::drawEntities(sf::RenderWindow& window) const{
    for (const auto& ent : entities) {
        ent->draw(window);
    }
}

std::optional<std::pair<std::string, sf::Vector2f>> Map::tryTeleport(const sf::FloatRect& playerBounds) const {
    for (const auto& ent : entities) {
        if (const auto portalPtr = dynamic_cast<const Portal*>(ent.get())) {
            if (portalPtr->getBounds().findIntersection(playerBounds).has_value()) {
                if (!portalPtr->isObstacle()) {
                    return portalPtr->teleportDestination();
                }
            }
        }
    }
    return std::nullopt;
}

const float Map::TILE_SIZE = 96.0f;

sf::Vector2f Map::gridToWorld(const int x, const int y) {
    return {static_cast<float>(x) * TILE_SIZE,
            static_cast<float>(y) * TILE_SIZE};
}

std::pair<float, float> Map::generateRandomEnemySpawn() const{
    if (enemySpawns.empty()) {
        throw MapEntityException("Symbol 'E' missing form map generation. No existing enemy spawns.",
            {-999.f, -999.f});
    }

    const int randomIndex = Game::generateRandomInt(0, static_cast<int>(enemySpawns.size()) - 1);
    auto [x, y] = enemySpawns[randomIndex];

    float spawnX = x * TILE_SIZE;
    float spawnY = y * TILE_SIZE;

    return {spawnX, spawnY};
}

void Map::handleEnemyRespawn(const int enemiesDied) {
    const int enemiesRequested = enemiesDied;
    if (enemiesRequested <= 0) return;

    const int currentEnemies = Enemy::getActiveEnemyCount();
    const int slotsAvailable = maxEnemiesAllowed - currentEnemies;

    if (slotsAvailable <= 0) return;

    const int countToSpawn = std::min(enemiesRequested, slotsAvailable);
    spawnAdditionalEnemies(countToSpawn);
}

void Map::initializeWithPlayer(Player& player) {
    playerTarget = &player;

    auto [x, y] = playerSpawn;
    const float spawnX = x * TILE_SIZE;
    const float spawnY = y * TILE_SIZE;
    player.spawn(spawnX, spawnY);

    if (!hasSpawnedEnemies) {
        spawnEnemies();
        hasSpawnedEnemies = true;
    }
}

void Map::initializeWithExistingPlayer(Player &player) {
    playerTarget = &player;

    if (!hasSpawnedEnemies) {
        spawnEnemies();
        hasSpawnedEnemies = true;
    }
}

void Map::cleanupAndRespawn() {
    int deadCount = 0;

    std::vector<std::unique_ptr<Entity>> drops;

    std::erase_if(entities, [&](const std::unique_ptr<Entity>& en) {
        if (!en->isAlive()) {
            if (const auto* enemy = dynamic_cast<Enemy*>(en.get())) {
                if (playerTarget) {
                    enemy->grantReward(*playerTarget);
                    deadCount++;
                }
                if (Game::generateRandomInt(1, 100) <= 25) {
                    try {
                        constexpr float groundOffsetY = 40;
                        auto drop = PickupFactory::create(
                            PickupType::Ammo,
                            enemy->getPos().x,
                            enemy->getPos().y - groundOffsetY,
                            resManager,
                            playingSounds
                        );

                        if (drop) {
                            drops.push_back(std::move(drop));
                        }
                    } catch (const ResourceException& e) {
                        std::cout << "[Drop Error] " << e.what() << std::endl;
                    }
                }
            }
            return true;
        }
        return false;
    });

    for (auto& drop : drops) {
        try {
            spawnEntityAt(std::move(drop));
        } catch (const MapEntityException& e) {
            std::cout << "Could not drop pickup" << e.what();
        }
    }

    solidEntitiesCache.clear();
    for (const auto& ent : entities) {
        if (ent && ent->isObstacle()) {
            solidEntitiesCache.push_back(ent.get());
        }
    }
    if (deadCount > 0) {
        handleEnemyRespawn(deadCount);
    }
}

Map::~Map() { std::cout << "S a apelat destructor Map \n";}

std::pair<float, float> Map::findSpawnLocation(const char symbol) const {
    for (size_t y = 0; y < mapLayout.size(); ++y) {
        for (size_t x = 0; x < mapLayout[y].size(); ++x) {
            if (mapLayout[y][x] == symbol) {
                return {
                    static_cast<float>(x) * TILE_SIZE,
                    static_cast<float>(y) * TILE_SIZE
                };
            }
        }
    }
    return { -1.0f, -1.0f };
}

void Map::placeEntity(Entity& entity, const char mapSymbol) const {
    auto [x, y] = findSpawnLocation(mapSymbol);

    if (x < 0 || y < 0) {
        if (mapSymbol == 'C' && playerTarget) {
            const sf::Vector2f pPos = playerTarget->getPos();
            if (auto* pet = dynamic_cast<Pet*>(&entity)) {
                pet->teleport(pPos.x, pPos.y);
            }
        }
        return;
    }

    if (auto* pet = dynamic_cast<Pet*>(&entity)) {
        pet->teleport(x, y);
    }
    else if (auto* player = dynamic_cast<Player*>(&entity)) {
        player->spawn(x, y);
    }
}

bool Map::isWall(const sf::FloatRect& bounds, const bool checkEntities) const {
    const int startX = static_cast<int> (bounds.position.x / TILE_SIZE);
    const int startY = static_cast<int> (bounds.position.y / TILE_SIZE);
    const int endX = static_cast<int> ((bounds.position.x + bounds.size.x) / TILE_SIZE);
    const int endY = static_cast<int> ((bounds.position.y + bounds.size.y) / TILE_SIZE);

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