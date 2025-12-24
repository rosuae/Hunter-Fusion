#include "Entity.h"
#include <cmath>

Entity::Entity(std::string n, const float x, const float y, const float spd, const float grav, sf::Texture& tex, const int w, const int h)
    :
    name{std::move(n)},
    alive{true},
    max_health{100},
    health{max_health},
    hitboxWidth{w},
    hitboxHeight{h},
    posX{x},
    posY{y},
    speed{spd},
    gravity{grav},
    texture{&tex},
    sprite{*texture} {
}

void Entity::setPosition(const float x, const float y) {
    posX = x;
    posY = y;
    updateSpritePosition();
}

void Entity::updateSpritePosition() {
    sprite.setPosition(sf::Vector2f(std::floor(posX), std::floor(posY)));
}

void Entity::checkDeath() {
    if (health <= 0) {
        health = 0;
        alive = false;
    }
}

void Entity::updateHitbox() {
    hitbox.position.x = posX - static_cast<float>(hitboxWidth) / 2.0f;
    hitbox.position.y = posY - static_cast<float>(hitboxHeight);
    hitbox.size.x = static_cast<float>(hitboxWidth);
    hitbox.size.y = static_cast<float>(hitboxHeight);
}

void Entity::draw(sf::RenderWindow& window) const {
    if (!alive) return;
    window.draw(sprite);
}

void Entity::behavior(const float deltaTime, const Map &map) {
    if (!alive) return;

    doBehavior(deltaTime, map);
    updateSpritePosition();
    checkDeath();
}

void Entity::tryHit(const int damageAmount) {
    if (!alive) return;

    takeDamage(damageAmount);
    checkDeath();
}

bool Entity::isAlive() const{
    return alive;
}

sf::FloatRect Entity::getBounds() const {
    if (!alive) return sf::FloatRect{};
    return doGetBounds();
}

sf::Vector2f Entity::getPos() const {
    return {posX, posY};
}