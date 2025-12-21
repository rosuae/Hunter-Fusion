//
// Created by rosua on 11/28/2025.
//

#ifndef OOP_HUD_H
#define OOP_HUD_H

#include <SFML/Graphics.hpp>
#include "Player.h"
#include "ResourceManager.h"

class Player;
class Weapon;

class Hud {

    static constexpr float BAR_OFFSET_X = 133.f;
    static constexpr float BAR_OFFSET_Y = 44.f;
    static constexpr float AMMO_OFFSET_X = 647.f;
    static constexpr float AMMO_OFFSET_Y = 35.f;

    static constexpr unsigned MINIMAP_WIDTH = 250u;
    static constexpr unsigned MINIMAP_HEIGHT = 150u;
    static constexpr float MINIMAP_SCREEN_X = 50.f;
    static constexpr float MINIMAP_SCREEN_Y = 50.f;

    const Player& m_player;

    sf::Sprite m_backgroundSprite;
    sf::RectangleShape m_healthBar;
    sf::Vector2f m_mapPixelSize;

    sf::Text m_healthText;
    sf::Text m_ammoText;

    float m_barMaxWidth;
    float m_barHeight;

    sf::RenderTexture m_minimapTexture;
    sf::Sprite m_minimapSprite;
    sf::View m_minimapView;

    sf::VertexArray m_minimapGrid;
    sf::CircleShape m_playerDot;

    float m_minimapTileSize;
    sf::RectangleShape m_minimapBorder;

public:
    Hud(const Player& player, ResourceManager& resManager);

    void setMapData(const std::vector<std::string>& mapLayout);
    void update();
    void render(sf::RenderWindow& window);
};

#endif //OOP_HUD_H