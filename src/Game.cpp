#include "Game.h"
#include "Portal.h"
#include "Enemy.h"
#include "Player.h"
#include "Map.h"
#include "settingsMenuState.h"
#include "Camera.h"
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

    auto tempWeapon = std::make_unique<Weapon>(
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

    std::string initialMapPath = "assets/textures/map/harta.txt";
    m_currentMapPath = initialMapPath;

    auto tempMap = std::make_unique<Map>(
            "CurrentRoom",
            initialMapPath,
            m_resManager,
            m_playingSounds
        );
    m_map = std::move(tempMap);

    sf::Vector2f spawnPos = m_map->getPlayerWorldSpawn();

    m_player = std::make_unique<Player>(
            playerName,
            m_resManager.getTexture("samussheet.png"),
            spawnPos.x,
            spawnPos.y,
            m_playingSounds,
            m_resManager.getSound("jump.wav"),
            std::move(tempWeapon)
        );

    m_map->initializeWithExistingPlayer(*m_player);

    m_window.create(sf::VideoMode({m_width, m_height}), "Hunter Fusion", settings.GetWindowStyle());
    m_window.setFramerateLimit(90);
    // m_window.setVerticalSyncEnabled(true);

    m_camera = std::make_unique<Camera>(m_width, m_height, *m_player, m_resManager);
    m_camera->snapToPlayer();
}

void Game::loadLevel(const std::string& mapFile, const sf::Vector2f spawnPos) {
    auto tempMap = std::make_unique<Map>(
        "CurrentRoom",
        mapFile,
        m_resManager,
        m_playingSounds
    );

    m_map = std::move(tempMap);

    if (m_player) {
        if (spawnPos.x < 0 && spawnPos.y < 0) {
            m_map->initializeWithPlayer(*m_player);
        } else {
            m_player->spawn(spawnPos.x, spawnPos.y);
            m_map->initializeWithExistingPlayer(*m_player);
        }

        if (m_camera) {
            m_camera->snapToPlayer();
        }
    }

    m_clock.restart();
}

void Game::loadLevel(const std::pair<std::string, sf::Vector2f>& nextDestination) {
    std::string nextMapPath = nextDestination.first;
    const sf::Vector2f spawnPos = nextDestination.second;

    if (m_map) {
        if (!m_currentMapPath.empty()) {
            m_savedMaps[m_currentMapPath] = std::move(m_map);
        }
    }

    if (m_player) {
        m_player->resetWeaponProjectiles();
    }
    auto it = m_savedMaps.find(nextMapPath);

    if (it != m_savedMaps.end()) {
        m_map = std::move(it->second);
        m_savedMaps.erase(it);
    }
    else {
        auto tempMap = std::make_unique<Map>(
            "CurrentRoom",
            nextMapPath,
            m_resManager,
            m_playingSounds
        );
        m_map = std::move(tempMap);
    }

    m_currentMapPath = nextMapPath;

    if (m_player) {
        if (spawnPos.x < 0 && spawnPos.y < 0) {
            m_map->initializeWithExistingPlayer(*m_player);
        } else {
            m_player->spawn(spawnPos.x, spawnPos.y);
            m_map->initializeWithExistingPlayer(*m_player);
        }
        if (m_camera) {
            m_camera->snapToPlayer();
        }
    }

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
                    m_player->reload();
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

            if (isShooting) {
                m_player->fire(shootDirection);
            }
        }
    }
}

void Game::update(float deltaTime) {
    m_player->behavior(deltaTime, *m_map);
    m_map->updateEntities(deltaTime);

    handleCollisions();
    updateSounds();
    updateCamera(deltaTime);
}

void Game::handleCollisions() {
    if (const auto destination = m_map->tryTeleport(m_player->getBounds())) {
        loadLevel(*destination);
        return;
    }

    m_map->processProjectileCollisions();

    const int totalDamage = m_map->processEnemyAttacks();
    if (totalDamage > 0 && m_playerDamageCooldown.getElapsedTime().asSeconds() > 1.f) {
        m_player->tryHit(totalDamage);
        m_playerDamageCooldown.restart();
    }

    m_map->cleanupAndRespawn();
}

void Game::updateCamera(float deltaTime) const {
    if (!m_player) return;
    m_camera->followPlayer(deltaTime);
}

void Game::updateSounds() {
    m_playingSounds.remove_if([](const sf::Sound& Sound_) {
        return Sound_.getStatus() == sf::Sound::Status::Stopped;
    });
}

void Game::render() {
    m_window.clear(sf::Color::Transparent);

    if (!m_map || !m_player) {
        m_window.display();
        return;
    }

    m_camera->prepareScene(m_window);
    m_map->drawMap(m_window);
    m_player->draw(m_window);
    m_map->drawEntities(m_window);
    m_camera->drawHud(m_window);
    m_window.display();
}

int Game::generateRandomInt(const int min, const int max) {
    static std::mt19937 gen(std::random_device{}());
    return std::uniform_int_distribution(min, max)(gen);
}