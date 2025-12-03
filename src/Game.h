#pragma once

#include <list>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>

#include "Map.h"
#include "Enemy.h"
#include "Player.h"
#include "settingsMenuState.h"
#include "Camera.h"

class Game {

    void instanceObjects();
    void loadLevel(const std::string& mapFile, sf::Vector2f spawnPos = sf::Vector2f(-1.f, -1.f));
    void handleEvents();
    void update(float deltaTime);
    void render();

    void updateEntities(float deltaTime);
    void handleCollisions();
    void updateCamera(float deltaTime);
    void updateSounds();

    sf::RenderWindow m_window;
    std::unique_ptr<Camera> m_camera;
    unsigned int m_width;
    unsigned int m_height;

    ResourceManager& m_resManager;
    settingsMenuState settings;

    std::unique_ptr<Player> m_player;
    std::unique_ptr<Weapon> m_playerWeapon;
    std::unique_ptr<Map> m_map;
    // std::shared_ptr<Weapon> m_enemyWeapon;

    sf::Clock m_clock;
    sf::Clock m_playerDamageCooldown;
    sf::Clock m_damageClock;

    std::list <sf::Sound> m_playingSounds;
public:
    Game();
    ~Game();
    void run();
};