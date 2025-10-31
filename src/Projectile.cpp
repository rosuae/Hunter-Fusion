#include "Projectile.h"

void Projectile::setupSprite(const sf::Texture& tex) {
    sprite.setOrigin(sf::Vector2f(static_cast<float>(tex.getSize().x),
                                  static_cast<float>(tex.getSize().y) / 2.f));
    sprite.scale(sf::Vector2f(1.5f, 1.5f));
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

void Projectile::updatePosition(float deltaTime) {
    position.x += direction.x * speed * deltaTime;
    position.y += direction.y * speed * deltaTime;
    sprite.setPosition(position);
}

Projectile::Projectile(std::string n, int d, const sf::Texture& tex, sf::Vector2f playerPos, sf::Vector2f targetPos):
    nume{std::move(n)},
    dmg{d},
    speed{1000.f},
    active{true},
    position{playerPos},
    sprite{tex}
{
    calculateDirection(playerPos, targetPos);
    setupSprite(tex);
    std::cout<<"Constructor proiectil \n";
}

void Projectile::drawProjectile(sf::RenderWindow& window) const{
    window.draw(sprite);
}

void Projectile::projectileTravel (float deltaTime) {
    updatePosition(deltaTime);
}

void Projectile::deactivate() {
    active = false;
}

Projectile::~Projectile() { std::cout << "S a apelat destructor projectile \n";}

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
    return position.x < -100 || position.x > maxX + 100 || position.y < -100 || position.y > maxY + 100;
}

bool Projectile::shouldBeRemoved(float maxX, float maxY) const {
    return !active || isOutOfBounds(maxX, maxY);
}