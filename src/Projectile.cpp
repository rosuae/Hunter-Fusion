#include "Projectile.h"
#include "Map.h"
#include "Player.h"

void Projectile::setupSprite(const sf::Texture& tex) {
    sprite.setOrigin(sf::Vector2f(static_cast<float>(tex.getSize().x),
                                  static_cast<float>(tex.getSize().y) / 2.f));
    sprite.scale(sf::Vector2f(1.f, 1.f));
}

void Projectile::calculateDirection(sf::Vector2f playerPos, sf::Vector2f targetPos) {
    float dirX = targetPos.x - playerPos.x;
    float dirY = targetPos.y - playerPos.y;
    float length = std::sqrt(dirX * dirX + dirY * dirY);

    if (length != 0) {
        direction.x = dirX / length;
        direction.y = dirY / length;
    } else {
        direction = sf::Vector2f(1.f, 0.f);
    }
}

void Projectile::updatePosition(float deltaTime, const Map& map) {

    sf::FloatRect localBounds = sprite.getLocalBounds();
    float spriteWidth = localBounds.size.x;
    float spriteHeight = localBounds.size.y;

    position.x += direction.x * speed * deltaTime;
    position.y += direction.y * speed * deltaTime;

    float centerX = position.x - spriteWidth / 2.f;
    float centerY = position.y - spriteHeight / 2.f;

    sf::FloatRect testBoundsX (
        sf::Vector2f(centerX, centerY),
        sf::Vector2f(spriteWidth, spriteHeight)
        );

    if (map.isWall(testBoundsX))
        deactivate();

    sprite.setPosition(position);
}

Projectile::Projectile(std::string n, int d, const sf::Texture& tex, sf::Vector2f playerPos, sf::Vector2f targetPos):
    nume{std::move(n)},
    dmg{d},
    speed{1500.f},
    active{true},
    position{playerPos},
    sprite{tex}
{
    calculateDirection(playerPos, targetPos);
    setupSprite(tex);
}

void Projectile::drawProjectile(sf::RenderWindow& window) const{
    window.draw(sprite);
}

void Projectile::projectileTravel (float deltaTime, const Map& map) {
    updatePosition(deltaTime, map);
}

void Projectile::deactivate() {
    active = false;
}

sf::FloatRect Projectile::getBounds() const {
    return sprite.getGlobalBounds();
}

int Projectile::getDamage() const {
    return dmg;
}

bool Projectile::isActive() const {
    return active;
}

bool Projectile::isOutOfBounds(float maxX, float maxY) const {
    return position.x < -100 || position.x > maxX + 100 || position.y < -100 || position.y > maxY + 3000;
}

bool Projectile::shouldBeRemoved(float maxX, float maxY) const {
    return !active || isOutOfBounds(maxX, maxY);
}