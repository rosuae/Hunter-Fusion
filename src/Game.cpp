#include "Game.h"
#include "ResourceManager.h"
#include <iostream>
#include <fstream>
#include <random>

namespace {
    bool intersects(const sf::FloatRect& rect1, const sf::FloatRect& rect2) {
        return rect1.findIntersection(rect2).has_value();
    }

    int randomInt(const int min, const int max) {
        static std::mt19937 gen(std::random_device{}());
        return std::uniform_int_distribution(min, max)(gen);
    }
}

Game::Game() :
    m_resManager(ResourceManager::Instance()) {
    std::ifstream fin("date.txt");
    if (!fin.is_open()) {
        std::cout << "Eroare la deschiderea fisierului date.txt \n";
    }

    std::string playerName, playerWeapon, projectileName, mapName, enemyWeapon, enemyProj;
    fin >> playerName >> playerWeapon >> projectileName >> mapName >> enemyWeapon >> enemyProj;

    m_player = std::make_unique<Player>(
    playerName,
    m_resManager.getTexture("samussheet.png"),
    m_playingSounds,
    m_resManager.getSound("jump.wav")
    );
    m_playerWeapon = std::make_unique<Weapon>(
    playerWeapon,
    projectileName,
    25,
    m_resManager.getTexture("projectile.png"),
    120
    );
    m_map = std::make_unique<Map>(
    mapName,
    "assets/textures/map/harta.txt",
    m_resManager
    );
    m_enemyWeapon = std::make_shared<Weapon>(
    enemyWeapon,
    enemyProj,
    10,
    m_resManager.getTexture("projectile.png"),
    1000);

    std::vector<std::string> enemyNames;
    std::string enemyName;
    while (fin >> enemyName) {
        enemyNames.push_back(enemyName);
    }
    fin.close();

    for (const auto& name: enemyNames) {
        auto newEnemy = std::make_unique<Enemy>(
            name,
            m_enemyWeapon,
            static_cast<float>(randomInt(100, 1920)),
            static_cast<float>(randomInt(99, 400)),
            m_resManager.getTexture("enemy.png"),
            m_playingSounds,
            m_resManager.getSound("enemydamage.wav"),
            m_resManager.getSound("enemydeath.wav")
        );

        newEnemy->setTarget(m_player.get());

        m_map->addEntity(std::move(newEnemy));
    }

    const sf::VideoMode desktop = sf::VideoMode::getDesktopMode();
    m_width = desktop.size.x;
    m_height = desktop.size.y;

    m_window.create(sf::VideoMode({m_width, m_height}, desktop.bitsPerPixel), "Hunter Fusion", sf::Style::Default, sf::State::Fullscreen);
    std::cout << "Fereastra a fost creată\n";
    m_window.setVerticalSyncEnabled(true);

    m_camera = sf::View(sf::FloatRect(sf::Vector2f(0.f, 0.f), sf::Vector2f(static_cast<float>(m_width), static_cast<float>(m_height))));
    m_window.setView(m_camera);
    m_cameraPos = m_player->getPos();

}
Game::~Game() {
    for (auto& sound : m_playingSounds) {
        sound.stop();
    }
    m_playingSounds.clear();
}
void Game::run() {
    while (m_window.isOpen()) {
        float deltaTime = m_clock.restart().asSeconds();
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
                m_playerWeapon->reload(m_playingSounds, m_resManager.getSound("reload.wav"));
            }
        }

        if (const auto* mousePress = event->getIf<sf::Event::MouseButtonPressed>()) {
            if (mousePress->button == sf::Mouse::Button::Left && m_playerWeapon->canFire(*m_player) && m_player->isAlive()) {
                sf::Vector2i mousePixel = sf::Mouse::getPosition(m_window);
                sf::Vector2f mouseWorld = m_window.mapPixelToCoords(mousePixel);

                m_playerWeapon->fire(*m_player, m_player->getWeaponTipPos(), mouseWorld, m_playingSounds, m_resManager.getSound("shoot.wav"));
                m_player->shootAnimation();
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
        for (auto& en : m_map->getEntities()) {
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

            newEnemy->setPosition(
                static_cast<float>(randomInt(100, 1920)),
                static_cast<float>(randomInt(99, 400))
            );

            m_map->addEntity(std::move(newEnemy));
        }
    }
}

void Game::updateCamera(float deltaTime) {
    sf::Vector2f targetPos = m_player->getPos();
    m_cameraPos.y += (targetPos.y - m_cameraPos.y - static_cast<float>(m_height) / 6) * m_cameraSpeed * deltaTime;

    m_camera.setCenter(m_cameraPos);
    m_window.setView(m_camera);
}

void Game::updateSounds() {
    m_playingSounds.remove_if([](const sf::Sound& Sound_) {
        return Sound_.getStatus() == sf::Sound::Status::Stopped;
    });
}

void Game::render() {
    m_window.clear(sf::Color::Black);

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