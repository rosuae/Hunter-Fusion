#include "Game.h"
#include "ResourceManager.h"
#include "GameExceptions.h"
#include <iostream>
#include <fstream>
#include <random>

namespace {
    bool intersects(const sf::FloatRect& rect1, const sf::FloatRect& rect2) {
        return rect1.findIntersection(rect2).has_value();
    }
}

void Game::instanceObjects() {
    std::ifstream fin("date.txt");
    if (!fin.is_open()) {
        throw ResourceException("Couldn't load file: date.txt");
    }

    std::string playerName, playerWeapon, projectileName, mapName, enemyWeapon, enemyProj;
    if (!(fin >> playerName >> playerWeapon >> projectileName >> mapName >> enemyWeapon >> enemyProj)) {
        throw ResourceException("File: date.txt ; incomplete or currupted");
    }

    m_map = std::make_unique<Map>(
    mapName,
    "assets/textures/map/harta.txt",
    m_resManager
    );

    auto [xPlayer, yPlayer] = m_map->getPlayerSpawn();
    m_player = std::make_unique<Player>(
    playerName,
    m_resManager.getTexture("samussheet.png"),
    xPlayer,
    yPlayer,
    m_playingSounds,
    m_resManager.getSound("jump.wav")
    );

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

    m_enemyWeapon = std::make_shared<Weapon>(
    enemyWeapon,
    enemyProj,
    10,
    m_resManager.getTexture("projectile.png"),
    1000,
    m_playingSounds,
    m_resManager.getSound("shoot.wav"),
    m_resManager.getSound("reload.wav")
    );

    std::vector<std::string> enemyNames;
    std::string enemyName;
    while (fin >> enemyName) {
        enemyNames.push_back(enemyName);
    }
    fin.close();

    for (const auto& name: enemyNames) {

        auto [x, y] = m_map->generateEnemySpawn();
        auto newEnemy = std::make_unique<Enemy>(
            name,
            m_enemyWeapon,
            x,
            y,
            m_resManager.getTexture("enemy.png"),
            m_playingSounds,
            m_resManager.getSound("enemydamage.wav"),
            m_resManager.getSound("enemydeath.wav")
        );

        newEnemy->setTarget(m_player.get());
        try{
        m_map->addEntity(std::move(newEnemy));
        }
        catch (const MapEntityException& e) {
            std::cout << e.what() << std::endl;
        }
    }

    m_window.create(sf::VideoMode({m_width, m_height}), "Hunter Fusion", settings.GetWindowStyle());
    m_window.setFramerateLimit(90);
    // m_window.setVerticalSyncEnabled(true);

    m_camera = sf::View(sf::FloatRect(sf::Vector2f(0.f, 0.f), sf::Vector2f(static_cast<float>(m_width), static_cast<float>(m_height))));
    m_window.setView(m_camera);
    m_cameraPos = m_player->getPos();
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
    m_resManager.cleanup();
    for (auto& sound : m_playingSounds) {
        sound.stop();
    }
    m_playingSounds.clear();
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
            if (keyPress->scancode == sf::Keyboard::Scancode::R) {
                try {
                    m_playerWeapon->reload();
                }
                catch (const InvalidActionException& e) {
                    std::cout << e.what() << std::endl;
                }
            }
        }

        if (const auto* keyPress = event->getIf<sf::Event::KeyPressed>()) {
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

                m_playerWeapon->fire(*m_player, shootDirection);
            }
        }
    }
}

void Game::update(float deltaTime) {
    updateEntities(deltaTime);
    handleCollisions();
    updateCamera(deltaTime);
    updateSounds();
}

void Game::updateEntities(float deltaTime) {

    m_player->behavior(deltaTime, *m_map);

    m_map->updateEntities(deltaTime);

    m_playerWeapon->update(deltaTime);
    m_playerWeapon->updateProjectiles(deltaTime, *m_map);

    if (m_player->isHit()) {
        float elapsed = m_damageClock.getElapsedTime().asSeconds();
        if (elapsed < 0.3f) {
            int alpha = static_cast<int>(120 * (1.f - elapsed / 0.3f));
            m_player->alphaDamageEffect(alpha);
        } else {
            m_player->setHit(false);
            m_player->resetDamageEffect();
        }
    }
}

void Game::handleCollisions() {
    for (auto& proj : m_playerWeapon->getProjectiles()) {
        if (!proj.isActive()) continue;
        for (const auto& en : m_map->getEntities()) {
            if (en->isAlive() && intersects(proj.getBounds(), en->getBounds())) {
                en->takeDamage(proj.getDamage());
                proj.deactivate();
                break;
            }
        }
    }

    int totalDamageThisFrame = 0;
    for (const auto& en : m_map->getEntities()) {
        if (const auto enPtr = dynamic_cast<const Enemy*>(en.get()))
            if (enPtr->isAlive() && m_player->isAlive() && intersects(m_player->getBounds(), enPtr->getBounds())) {
                totalDamageThisFrame += enPtr->getContactDamage();
            }
    }

    if (totalDamageThisFrame > 0 && m_playerDamageCooldown.getElapsedTime().asSeconds() > 1.f) {
        m_player->takeDamage(totalDamageThisFrame);
        m_player->setHit(true);
        m_damageClock.restart();
        m_playerDamageCooldown.restart();
    }

    int enemiesDied = m_map->removeDeadEntities();
    int enemiesToSpawn = enemiesDied * 2;

    if (enemiesToSpawn > 0) {
        Enemy prototype(
            "Metroid",
            m_enemyWeapon,
            0, 0,
            m_resManager.getTexture("enemy.png"),
            m_playingSounds,
            m_resManager.getSound("enemydamage.wav"),
            m_resManager.getSound("enemydeath.wav")
        );
        prototype.setTarget(m_player.get());

        for (int i = 0; i < enemiesToSpawn; ++i) {
            auto newEnemy = std::make_unique<Enemy>(prototype);
            auto [x, y] = m_map->generateEnemySpawn();
            newEnemy->setPosition(x, y);
            try {
                m_map->addEntity(std::move(newEnemy));
            }
            catch (const MapEntityException& e) {
                std::cout << e.what() << std::endl;
            }
        }
    }
}

void Game::updateCamera(float deltaTime) {
    sf::Vector2f targetPos = m_player->getPos();

    constexpr float deadZone = 8.f;
    float diffX = targetPos.x - m_cameraPos.x;
    float diffY = targetPos.y - m_cameraPos.y - static_cast<float>(m_height) / 6;

    if (std::abs(diffX) > deadZone) {
        m_cameraPos.x += diffX * m_cameraSpeed * deltaTime;
    } else {
        m_cameraPos.x = targetPos.x;
    }

    if (std::abs(diffY) > deadZone) {
        m_cameraPos.y += diffY * m_cameraSpeed * deltaTime;
    } else {
        m_cameraPos.y = targetPos.y - static_cast<float>(m_height) / 6;
    }

    m_camera.setCenter(m_cameraPos);
    m_window.setView(m_camera);
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

    m_player->drawDamageEffect(m_window);

    m_window.display();
}