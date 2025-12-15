//
// Created by rosua on 11/28/2025.
//

#include "Camera.h"
#include "Hud.h"
#include <algorithm>
#include <cmath>

Camera::Camera(const unsigned int width, const unsigned int height, const Player& player, ResourceManager& resManager)
    : m_focusedPlayer{player}, m_smoothSpeed{10.f} {
    float viewWidth = static_cast<float>(width) / 1.3f;
    float viewHeight = static_cast<float>(height) / 1.3f;

    m_view.setSize({viewWidth, viewHeight});
    m_verticalOffset = 0.f;

    sf::Vector2f startPos = player.getPos();
    startPos.y -= m_verticalOffset;

    m_currentPos = startPos;
    m_targetPos = startPos;

    m_view.setCenter(m_currentPos);

    m_threshold.x = viewWidth / 7.f;
    m_threshold.y = viewHeight / 4.f;

    m_hud = std::make_unique<Hud>(player, resManager);
}

Camera::~Camera() = default;

void Camera::followPlayer(const float deltaTime) {
    const sf::Vector2f playerPos = m_focusedPlayer.getPos();

    const float limitRight = m_targetPos.x + m_threshold.x;
    const float limitLeft  = m_targetPos.x - m_threshold.x;
    const float limitDown  = m_targetPos.y + m_threshold.y;
    const float limitUp    = m_targetPos.y - m_threshold.y;

    if (playerPos.x > limitRight) {
        m_targetPos.x += (playerPos.x - limitRight);
    } else if (playerPos.x < limitLeft) {
        m_targetPos.x += (playerPos.x - limitLeft);
    }

    float playerYWithOffset = playerPos.y - m_verticalOffset;
    if (playerYWithOffset > limitDown) {
        m_targetPos.y += (playerYWithOffset - limitDown);
    } else if (playerYWithOffset < limitUp) {
        m_targetPos.y += (playerYWithOffset - limitUp);
    }

    float moveFactor = m_smoothSpeed * deltaTime;

    if (moveFactor > 1.0f) moveFactor = 1.0f;

    m_currentPos.x = std::lerp(m_currentPos.x, m_targetPos.x, moveFactor);
    m_currentPos.y = std::lerp(m_currentPos.y, m_targetPos.y, moveFactor);

    sf::Vector2f roundedPos;
    roundedPos.x = std::round(m_currentPos.x);
    roundedPos.y = std::round(m_currentPos.y);

    m_view.setCenter(roundedPos);

    if (m_hud) {
        m_hud->update();
    }
}

void Camera::prepareScene(sf::RenderTarget& target) const {
    target.setView(m_view);
}

void Camera::drawHud(sf::RenderWindow& window) const{
    if (!m_hud) return;
    const sf::View worldView = window.getView();
    window.setView(window.getDefaultView());
    m_hud->render(window);

    window.setView(worldView);
}

void Camera::updateMinimap(const std::vector<std::string>& layout) const {
    if (m_hud) {
        m_hud->setMapData(layout);
    }
}

void Camera::snapToPlayer() {
    sf::Vector2f finalPos = m_focusedPlayer.getPos();
    finalPos.y -= m_verticalOffset;
    m_targetPos = finalPos;
    m_currentPos = finalPos;
    m_view.setCenter(m_currentPos);
}