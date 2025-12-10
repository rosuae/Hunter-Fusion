//
// Created by rosua on 11/28/2025.
//

#include "Camera.h"
#include "Hud.h"
#include <cmath>

Camera::Camera(unsigned int width, unsigned int height)
    : m_speed{5.f}, m_deadZone{8.f}
{
    float viewWidth = static_cast<float>(width) / 1.3f;
    float viewHeight = static_cast<float>(height) / 1.3f;

    m_view.setSize({viewWidth, viewHeight});
    m_view.setCenter({viewWidth / 2.f, viewHeight / 2.f});

    m_verticalOffset = static_cast<float>(height) / 5.f;
}

Camera::~Camera() = default;

void Camera::initHud(const std::unique_ptr<Player>& player, ResourceManager& resManager) {
    m_hud = std::make_unique<Hud>(player, resManager);
}

void Camera::update(float deltaTime, sf::Vector2f targetPosition) {
    sf::Vector2f finalTarget = targetPosition;
    finalTarget.y -= m_verticalOffset;

    float diffX = finalTarget.x - m_currentPos.x;
    float diffY = finalTarget.y - m_currentPos.y;

    if (std::abs(diffX) > m_deadZone) m_currentPos.x += diffX * m_speed * deltaTime;
    else m_currentPos.x = finalTarget.x;

    if (std::abs(diffY) > m_deadZone) m_currentPos.y += diffY * m_speed * deltaTime;
    else m_currentPos.y = finalTarget.y;

    m_view.setCenter(m_currentPos);

    if (m_hud) {
        m_hud->update();
    }
}

void Camera::drawHud(sf::RenderWindow& window) const{
    if (!m_hud) return;
    const sf::View worldView = window.getView();
    window.setView(window.getDefaultView());

    m_hud->render(window);
    window.setView(worldView);
}

void Camera::snapToTarget(sf::Vector2f rawTargetPosition) {
    sf::Vector2f finalPos = rawTargetPosition;
    finalPos.y -= m_verticalOffset;
    m_currentPos = finalPos;
    m_view.setCenter(m_currentPos);
}

const sf::View& Camera::getView() const {
    return m_view;
}