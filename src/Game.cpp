#include "Game.h"
#include "Portal.h"
#include "ResourceManager.h"
#include "GameExceptions.h"
#include <iostream>
#include <fstream>
#include <random>

void Game::instanceObjects() {
    std::ifstream fin("date.txt");
    if (!fin.is_open()) {
        throw ResourceException("Couldn't load file: date.txt");
    }

    std::string playerName, playerWeapon, projectileName;
    if (!(fin >> playerName >> playerWeapon >> projectileName)) {
        throw ResourceException("File: date.txt ; incomplete or currupted");
    }

    m_playerWeapon = std::make_unique<Weapon>(
    playerWeapon,
    projectileName,
    25,
    m_resManager.getTexture("projectile.png"),
    120,
    m_playingSounds,
    m_resManager.getSound("shoot.wav"),
    m_resManager.getSound("reload.wav")
    );

    fin.close();

    loadLevel("assets/textures/map/harta.txt");

    auto [xPlayer, yPlayer] = m_map->getPlayerSpawn();

    m_player = std::make_unique<Player>(
        playerName,
        m_resManager.getTexture("samussheet.png"),
        xPlayer,
        yPlayer,
        m_playingSounds,
        m_resManager.getSound("jump.wav")
    );

    m_map->setPlayerTarget(m_player.get());
    m_map->spawnEnemies();

    m_window.create(sf::VideoMode({m_width, m_height}), "Hunter Fusion", settings.GetWindowStyle());
    m_window.setFramerateLimit(90);
    // m_window.setVerticalSyncEnabled(true);

    m_camera = std::make_unique<Camera>(m_width, m_height);
    m_camera->snapToTarget(m_player->getPos());
    m_camera->initHud(m_player, m_playerWeapon, m_resManager);

    m_window.setView(m_camera->getView());
}

void Game::loadLevel(const std::string& mapFile, sf::Vector2f spawnPos) {
    auto tempMap = std::make_unique<Map>(
        "CurrentRoom",
        mapFile,
        m_resManager,
        m_playingSounds
    );

    m_map = std::move(tempMap);

    sf::Vector2f newRoomPos;
    if (spawnPos.x < 0 && spawnPos.y < 0) {
        auto [x, y] = m_map->getPlayerSpawn();
        newRoomPos = sf::Vector2f(x, y);
    } else {
        newRoomPos = spawnPos;
    }

    if (m_player) {
        m_player->spawn(newRoomPos.x, newRoomPos.y);
        m_camera->snapToTarget(newRoomPos);
    }

    m_map->setPlayerTarget(m_player.get());
    m_map->spawnEnemies();

    m_clock.restart();
}

Game::Game() :
    m_width{},
    m_height{},
    m_resManager{ResourceManager::Instance()} {
    settings.SetSettingsLauncher();
    m_width = settings.GetResolution().x;
    m_height = settings.GetResolution().y;
}

Game::~Game() {
    for (auto& sound : m_playingSounds) {
        sound.stop();
    }
    m_map.reset();
    m_playingSounds.clear();
    m_resManager.cleanup();
}

void Game::run() {
    instanceObjects();
    m_clock.restart();
    while (m_window.isOpen()) {
        float deltaTime = m_clock.restart().asSeconds();
        if (deltaTime > 0.1f) deltaTime = 0.1f;
        handleEvents();
        update(deltaTime);
        render();
    }
}

void Game::handleEvents() {
    while (const std::optional<sf::Event> event = m_window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) {
            m_window.close();
        }

        if (const auto* keyPress = event->getIf<sf::Event::KeyPressed>()) {
            if (keyPress->scancode == sf::Keyboard::Scancode::Escape) {
                m_window.close();
            }
            else if (keyPress->scancode == sf::Keyboard::Scancode::R) {
                try {
                    m_playerWeapon->reload();
                }
                catch (const InvalidActionException& e) {
                    std::cout << e.what() << std::endl;
                }
            }

            sf::Vector2f shootDirection;
            bool isShooting = false;

            if (keyPress->scancode == sf::Keyboard::Scancode::Left) {
                shootDirection = sf::Vector2f(-1.f, 0.f);
                isShooting = true;
            }
            else if (keyPress->scancode == sf::Keyboard::Scancode::Right) {
                shootDirection = sf::Vector2f(1.f, 0.f);
                isShooting = true;
            }
            else if (keyPress->scancode == sf::Keyboard::Scancode::Up) {
                shootDirection = sf::Vector2f(0.f, -1.f);
                isShooting = true;
            }

            if (isShooting && m_playerWeapon->canFire(*m_player) && m_player->isAlive()) {
                m_player->shoot(shootDirection);
                m_playerWeapon->fire(*m_player, shootDirection);
            }
        }
    }
}

void Game::update(float deltaTime) {
    m_player->behavior(deltaTime, *m_map);
    m_playerWeapon->update(deltaTime);
    m_map->updateEntities(deltaTime);

    handleCollisions();

    m_playerWeapon->updateProjectiles(deltaTime, *m_map);

    updateSounds();
    updateCamera(deltaTime);
}

void Game::handleEnemyRespawn(const int enemiesDied) {
    const int enemiesRequested = enemiesDied * 2;
    if (enemiesRequested <= 0) return;

    constexpr int maxEnemies = 12;
    const int currentEnemies = Enemy::getActiveEnemyCount();
    const int slotsAvailable = maxEnemies - currentEnemies;

    if (slotsAvailable <= 0) return;
    const int countToSpawn = std::min(enemiesRequested, slotsAvailable);

    for (int i = 0; i < countToSpawn; ++i) {
        try {
            auto [x, y] = m_map->generateEnemySpawn();
            std::unique_ptr newEnemy = EnemyFactory::createEnemy(
            "Metroid",
             x, y,
             m_resManager,
             m_playingSounds,
             m_player.get()
            );
            m_map->addEntity(std::move(newEnemy));
        }
        catch (const MapEntityException& e) {
            std::cout << e.what() << std::endl;
        }
    }
}

void Game::handleCollisions() {
    const Portal* hitPortal = m_map->getPortalCollision(m_player->getBounds());

    if (hitPortal != nullptr && hitPortal->isOpen()) {
        const std::string nextMap = hitPortal->getNextMapFile();
        const sf::Vector2f nextSpawn = hitPortal->getNextPlayerSpawn();

        loadLevel(nextMap, nextSpawn);
        return;
    }

    for (auto& proj : m_playerWeapon->getProjectiles()) {
        if (!proj.isActive()) continue;
        for (const auto& en : m_map->getEntities()) {
            if (en->isAlive() && proj.getBounds().findIntersection(en->getBounds()).has_value()) {
                en->tryHit(m_playerWeapon->getDmg());
                proj.deactivate();
                break;
            }
        }
    }

    int totalDamageThisFrame = 0;
    for (const auto& en : m_map->getEntities()) {
        if (const auto enPtr = dynamic_cast<const Enemy*>(en.get()))
            if (enPtr->isAlive() && m_player->isAlive() && m_player->getBounds().findIntersection(enPtr->getBounds()).has_value()) {
                totalDamageThisFrame += enPtr->getContactDamage();
            }
    }

    if (totalDamageThisFrame > 0 && m_playerDamageCooldown.getElapsedTime().asSeconds() > 1.f) {
        m_player->tryHit(totalDamageThisFrame);
        m_playerDamageCooldown.restart();
    }

    if (const int enemiesDied = m_map->removeDeadEntities(); enemiesDied > 0) {
        handleEnemyRespawn(enemiesDied);
    }
}

void Game::updateCamera(float deltaTime) {
    if (!m_player) return;

    m_camera->update(deltaTime, m_player->getPos());
    m_window.setView(m_camera->getView());
}

void Game::updateSounds() {
    m_playingSounds.remove_if([](const sf::Sound& Sound_) {
        return Sound_.getStatus() == sf::Sound::Status::Stopped;
    });
}

void Game::render() {
    m_window.clear(sf::Color::Transparent);

    if (!m_map || !m_playerWeapon || !m_player) {
        m_window.display();
        return;
    }

    m_map->drawMap(m_window);
    m_playerWeapon->drawProjectiles(m_window);
    m_player->draw(m_window);
    m_map->drawEntities(m_window);
    m_camera->drawHud(m_window);
    m_window.display();
}

int Game::generateRandomInt(const int min, const int max) {
    static std::mt19937 gen(std::random_device{}());
    return std::uniform_int_distribution(min, max)(gen);
}