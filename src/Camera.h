#pragma once
#include <SFML/Graphics.hpp>
#include <memory>

class Hud;
class Player;
class Weapon;
class ResourceManager;

class Camera {
public:
    Camera(unsigned int width, unsigned int height, const Player& player, ResourceManager& resManager);
    ~Camera();

    void followPlayer(float deltaTime);
    void snapToPlayer();
    void prepareScene(sf::RenderTarget& target) const;
    void drawHud(sf::RenderWindow& window) const;
    void updateMinimap(const std::vector<std::string>& layout) const;

private:
    sf::View m_view;
    sf::Vector2f m_currentPos;
    sf::Vector2f m_targetPos;
    sf::Vector2f m_threshold;

    const Player& m_focusedPlayer;
    std::unique_ptr<Hud> m_hud;

    float m_smoothSpeed;
    float m_verticalOffset;
};