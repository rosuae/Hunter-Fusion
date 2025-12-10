#pragma once
#include <SFML/Graphics.hpp>
#include <memory>

class Hud;
class Player;
class Weapon;
class ResourceManager;

class Camera {
public:
    Camera(unsigned int width, unsigned int height);
    ~Camera();

    void initHud(const std::unique_ptr<Player>& player, ResourceManager& resManager);

    void update(float deltaTime, sf::Vector2f targetPosition);
    void drawHud(sf::RenderWindow& window) const;

    const sf::View& getView() const;
    void snapToTarget(sf::Vector2f targetPosition);

private:
    sf::View m_view;
    sf::Vector2f m_currentPos;
    std::unique_ptr<Hud> m_hud;

    float m_speed;
    float m_deadZone;
    float m_verticalOffset;
};