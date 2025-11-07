#pragma once

#include <list>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>

#include "Map.h"
#include "Enemy.h"
#include "Player.h"

class Game {

    void handleEvents();
    void update(float deltaTime);
    void render();

    void updateEntities(float deltaTime);
    void handleCollisions();
    void updateCamera(float deltaTime);
    void updateSounds();

    sf::RenderWindow m_window;
    sf::View m_camera;
    sf::Vector2f m_cameraPos;
    unsigned int m_width;
    unsigned int m_height;
    const float m_cameraSpeed = 5.f;

    ResourceManager& m_resManager;

    std::unique_ptr<Player> m_player;
    std::unique_ptr<Weapon> m_playerWeapon;
    std::unique_ptr<Map> m_map;
    std::unique_ptr<Weapon> m_enemyWeapon;

    sf::Clock m_clock;
    sf::Clock m_playerDamageCooldown;
    sf::Clock m_damageClock;

    std::list <sf::Sound> m_playingSounds;
public:

    Game();
    ~Game();
    void run();

};