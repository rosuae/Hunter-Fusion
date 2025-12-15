//
// Created by rosua on 11/28/2025.
//

#include "Hud.h"
#include "GameExceptions.h"
#include "Player.h"
#include "Weapon.h"
#include <string>
#include <algorithm>

#define BAR_OFFSET_X 133.f
#define BAR_OFFSET_Y 44.f
#define AMMO_OFFSET_X 647.f
#define AMMO_OFFSET_Y 35.f

#define MINIMAP_WIDTH 250u
#define MINIMAP_HEIGHT 150u
#define MINIMAP_SCREEN_X 50.f
#define MINIMAP_SCREEN_Y 50.f

Hud::Hud(const Player& player, ResourceManager& resManager) :
    m_player{player},
    m_backgroundSprite{resManager.getTexture("hud.png")},
    m_healthText{resManager.getFont("Metroid-Fusion.ttf")},
    m_ammoText{resManager.getFont("Metroid-Fusion.ttf")},
    m_barMaxWidth{483.f},
    m_barHeight{13.f},
    m_minimapTexture{},
    m_minimapSprite{m_minimapTexture.getTexture()},
    m_minimapGrid{sf::PrimitiveType::Triangles},
    m_minimapTileSize{6.0f}
{
    m_backgroundSprite.setPosition({1075.f, 10.f});
    m_healthBar.setFillColor(sf::Color(50, 205, 50));
    m_healthBar.setPosition({m_backgroundSprite.getPosition().x + BAR_OFFSET_X,
                            m_backgroundSprite.getPosition().y + BAR_OFFSET_Y});
    m_healthBar.setSize(sf::Vector2f(m_barMaxWidth, m_barHeight));

    m_healthText.setCharacterSize(25);
    m_healthText.setFillColor(sf::Color::Magenta);
    m_healthText.setPosition({m_healthBar.getPosition().x + 470.f, m_healthBar.getPosition().y + 20.f});
    m_healthText.setOutlineColor(sf::Color::Black);
    m_healthText.setOutlineThickness(1.f);

    m_ammoText.setCharacterSize(20);
    m_ammoText.setFillColor(sf::Color::Magenta);
    m_ammoText.setPosition({m_backgroundSprite.getPosition().x + AMMO_OFFSET_X,
                           m_backgroundSprite.getPosition().y + AMMO_OFFSET_Y});

    if (!m_minimapTexture.resize({MINIMAP_WIDTH, MINIMAP_HEIGHT})) {
        throw ResourceException("couldn't load minimap texture");
    }
    m_minimapSprite.setTexture(m_minimapTexture.getTexture(), true);
    m_minimapSprite.setPosition({MINIMAP_SCREEN_X, MINIMAP_SCREEN_Y});
    m_minimapBorder.setSize({static_cast<float>(MINIMAP_WIDTH), static_cast<float>(MINIMAP_HEIGHT)});
    m_minimapBorder.setPosition({MINIMAP_SCREEN_X, MINIMAP_SCREEN_Y});
    m_minimapBorder.setFillColor(sf::Color::Transparent);
    m_minimapBorder.setOutlineColor(sf::Color::White);
    m_minimapBorder.setOutlineThickness(2.f);

    m_minimapView.setSize({static_cast<float>(MINIMAP_WIDTH), static_cast<float>(MINIMAP_HEIGHT)});
    m_minimapView.zoom(0.7f);

    m_playerDot.setRadius(4.f);
    m_playerDot.setFillColor(sf::Color::Yellow);
    m_playerDot.setOrigin({4.f, 4.f});
}

void Hud::setMapData(const std::vector<std::string> &mapLayout) {
    m_minimapGrid.clear();
    if (mapLayout.empty()) return;

    const int rows = mapLayout.size();
    int cols = 0;
    for(const auto& line : mapLayout) {
        if(static_cast<int>(line.size()) > cols) cols = static_cast<int>(line.size());
    }

    m_mapPixelSize.x = static_cast<float>(cols) * m_minimapTileSize;
    m_mapPixelSize.y = static_cast<float>(rows) * m_minimapTileSize;

    for (int y = 0; y < rows; ++y) {
        for (auto x = 0u; x < mapLayout[y].size(); ++x) {
            if (mapLayout[y][x] == '#') {
                const float posX = static_cast<float>(x) * m_minimapTileSize;
                const float posY = static_cast<float>(y) * m_minimapTileSize;

                constexpr auto color = sf::Color(200, 200, 200, 200);
                m_minimapGrid.append(sf::Vertex{sf::Vector2f(posX, posY), color});
                m_minimapGrid.append(sf::Vertex{sf::Vector2f(posX + m_minimapTileSize, posY), color});
                m_minimapGrid.append(sf::Vertex{sf::Vector2f(posX, posY + m_minimapTileSize), color});

                m_minimapGrid.append(sf::Vertex{sf::Vector2f(posX + m_minimapTileSize, posY), color});
                m_minimapGrid.append(sf::Vertex{sf::Vector2f(posX + m_minimapTileSize, posY + m_minimapTileSize), color});
                m_minimapGrid.append(sf::Vertex{sf::Vector2f(posX, posY + m_minimapTileSize), color});
            }
        }
    }
}

void Hud::update() {
    int currentHP = m_player.getHealth();
    constexpr int maxHP = 100;

    if (currentHP < 0) currentHP = 0;
    const float hpRatio = static_cast<float>(currentHP) / static_cast<float>(maxHP);

    m_healthBar.setSize(sf::Vector2f(m_barMaxWidth * hpRatio, m_barHeight));

    m_healthText.setString( std::to_string(currentHP) + "  HEALTH");

    if (const Weapon* currentWeapon = m_player.getWeapon()){
        const int clip = currentWeapon->getAmmoInClip();
        const int reserve = currentWeapon->getTotalAmmo();
        const std::string ammoString = "AMMO " + std::to_string(clip) + " / " + std::to_string(reserve);
        m_ammoText.setString(ammoString);
    } else {
        m_ammoText.setString("NO WEAPON");
    }

    const sf::Vector2f playerWorldPos = m_player.getPos();
    constexpr float worldTileSize = 96.0f;

    float targetX = playerWorldPos.x / worldTileSize * m_minimapTileSize;
    float targetY = playerWorldPos.y / worldTileSize * m_minimapTileSize;

    m_playerDot.setPosition({targetX, targetY});

    const sf::Vector2f viewSize = m_minimapView.getSize();
    const float halfViewW = viewSize.x / 2.f;
    const float halfViewH = viewSize.y / 2.f;

    const float minX = halfViewW;
    const float minY = halfViewH;

    const float maxX = m_mapPixelSize.x - halfViewW;
    const float maxY = m_mapPixelSize.y - halfViewH;

    float clampedX, clampedY;

    if (m_mapPixelSize.x < viewSize.x) {
        clampedX = m_mapPixelSize.x / 2.f;
    } else {
        clampedX = std::clamp(targetX, minX, maxX);
    }

    if (m_mapPixelSize.y < viewSize.y) {
        clampedY = m_mapPixelSize.y / 2.f;
    } else {
        clampedY = std::clamp(targetY, minY, maxY);
    }
    m_minimapView.setCenter({clampedX, clampedY});
}

void Hud::render(sf::RenderWindow& window){
    m_minimapTexture.clear(sf::Color(0, 0, 0, 150));

    m_minimapTexture.setView(m_minimapView);
    m_minimapTexture.draw(m_minimapGrid);
    m_minimapTexture.draw(m_playerDot);

    m_minimapTexture.display();

    window.draw(m_backgroundSprite);
    window.draw(m_healthBar);
    window.draw(m_healthText);
    window.draw(m_ammoText);

    window.draw(m_minimapSprite);
    window.draw(m_minimapBorder);
}