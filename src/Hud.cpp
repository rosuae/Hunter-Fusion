//
// Created by rosua on 11/28/2025.
//

#include "Hud.h"
#include "GameExceptions.h"
#include <string>

#define BAR_OFFSET_X 133.f
#define BAR_OFFSET_Y 44.f
#define AMMO_OFFSET_X 647.f
#define AMMO_OFFSET_Y 35.f

Hud::Hud(const std::unique_ptr<Player>& player, const std::unique_ptr<Weapon>& weapon, ResourceManager& resManager)
    : m_player{player.get()},
      m_weapon{weapon.get()},
      m_backgroundSprite{resManager.getTexture("hud.png")},
      m_healthText{font},
      m_ammoText{font},
      m_barMaxWidth{483.f},
      m_barHeight{13.f}
{
    if (!font.openFromFile("assets/textures/Metroid-Fusion.ttf")) {
        throw ResourceException("Failed to load font inside HUD");
    }

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
}

void Hud::update() {
    if (!m_player || !m_weapon) return;

    int currentHP = m_player->getHealth();
    int maxHP = 100;

    if (currentHP < 0) currentHP = 0;
    float hpRatio = static_cast<float>(currentHP) / static_cast<float>(maxHP);

    m_healthBar.setSize(sf::Vector2f(m_barMaxWidth * hpRatio, m_barHeight));

    m_healthText.setString( std::to_string(currentHP) + "  HEALTH");

    int clip = m_weapon->getAmmoInClip();
    int reserve = m_weapon->getTotalAmmo();

    std::string ammoString = "AMMO " + std::to_string(clip) + " / " + std::to_string(reserve);
    m_ammoText.setString(ammoString);
}

void Hud::render(sf::RenderWindow& window) const{
    sf::View worldView = window.getView();

    window.setView(window.getDefaultView());

    window.draw(m_backgroundSprite);
    window.draw(m_healthBar);
    window.draw(m_healthText);
    window.draw(m_ammoText);
    window.setView(worldView);
}