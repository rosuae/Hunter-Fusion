//
// Created by rosua on 11/28/2025.
//

#ifndef OOP_HUD_H
#define OOP_HUD_H

#include <SFML/Graphics.hpp>
#include <memory>
#include "Player.h"
#include "ResourceManager.h"

class Hud {
    const Player* m_player;
    const Weapon* m_weapon;

    sf::Font font;

    sf::Sprite m_backgroundSprite;
    sf::RectangleShape m_healthBar;
    sf::RectangleShape m_healthBarBack;

    sf::Text m_healthText;
    sf::Text m_ammoText;

    float m_barMaxWidth;
    float m_barHeight;

public:
    Hud(const std::unique_ptr<Player>& player, ResourceManager& resManager);

    void update();
    void render(sf::RenderWindow& window) const;
};

#endif //OOP_HUD_H