#include "Projectile.h"
#include "Map.h"
#include "Entity.h"
#include <cmath>

void Projectile::setupSprite(const sf::Texture& tex) {
    sprite.setOrigin(sf::Vector2f(static_cast<float>(tex.getSize().x) / 2.f,
                                  static_cast<float>(tex.getSize().y) / 2.f));
    sprite.scale(sf::Vector2f(1.f, 1.f));
}

void Projectile::calculateDirection(const sf::Vector2f playerPos, const sf::Vector2f targetPos) {
    const float dirX = targetPos.x - playerPos.x;
    const float dirY = targetPos.y - playerPos.y;

    if (const float length = std::sqrt(dirX * dirX + dirY * dirY); length != 0) {
        direction.x = dirX / length;
        direction.y = dirY / length;
    } else {
        direction = sf::Vector2f(1.f, 0.f);
    }

    const sf::Angle angle = sf::radians(std::atan2(direction.y, direction.x));
    sprite.setRotation(angle);
}

bool Projectile::tryHit(Entity &target) {
    if (!active || !target.isAlive()) return false;

    if (getBounds().findIntersection(target.getBounds()).has_value()) {
        target.tryHit(dmg);
        active = false;
        return true;
    }
    return false;
}

void Projectile::update(const float deltaTime, const Map& map) {
    if (!active) return;

    const float moveAmount = speed * deltaTime;
    distanceTraveled += moveAmount;

    if (distanceTraveled > maxDistance) {
        active = false;
        return;
    }

    position += direction * speed * deltaTime;

    const sf::FloatRect localBounds = sprite.getLocalBounds();

    const float centerX = position.x - localBounds.size.x / 2.f;
    const float centerY = position.y - localBounds.size.y / 2.f;

    const sf::FloatRect testBoundsX (
        sf::Vector2f(centerX, centerY),
        sf::Vector2f(localBounds.size.x * 0.5f, localBounds.size.y * 0.5f)
        );

    if (map.isWall(testBoundsX, false))
        active = false;

    sprite.setPosition(position);
}

Projectile::Projectile(std::string n, const int d, const sf::Texture& tex, const sf::Vector2f playerPos, const sf::Vector2f targetPos):
    nume{std::move(n)},
    dmg{d},
    speed{2000.f},
    distanceTraveled{0.f},
    maxDistance{1200.f},
    active{true},
    position{playerPos},
    sprite{tex}
{
    calculateDirection(playerPos, targetPos);
    setupSprite(tex);
    sprite.setPosition(position);
}

void Projectile::drawProjectile(sf::RenderWindow& window) const{
    window.draw(sprite);
}

sf::FloatRect Projectile::getBounds() const {
    return sprite.getGlobalBounds();
}

bool Projectile::isActive() const {
    return active;
}

bool Projectile::shouldBeRemoved() const {
    return !active;
}