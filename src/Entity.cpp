#include "Entity.h"
#include <cmath>
#include <iostream>

Entity::Entity(std::string n, const float x, const float y, const float spd, const float grav, sf::Texture& tex, const int w, const int h)
    :
    alive{true},
    name{std::move(n)},
    health{100},
    hitboxWidth{w},
    hitboxHeight{h},
    posX{x},
    posY{y},
    speed{spd},
    gravity{grav},
    texture{&tex},
    sprite{*texture} {
}

Entity::~Entity() {
    std::cout << "S a apelat destructor Entity\n";
}

Entity::Entity (const Entity& other):
alive{other.alive},
name{other.name},
health{other.health},
hitboxWidth{other.hitboxWidth},
hitboxHeight{other.hitboxHeight},
posX{other.posX},
posY{other.posY},
speed{other.speed},
gravity{other.gravity},
texture{other.texture},
sprite{other.sprite}{
    std::cout << "S a apelat constructor copiere entity";
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

void Entity::behavior(float deltaTime, const Map &map) {
    if (!alive) return;

    doBehavior(deltaTime, map);
    updateSpritePosition();
    checkDeath();
}

void Entity::takeDamage(int damageAmount) {
    if (!alive) return;

    doTakeDamage(damageAmount);
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
void Entity::setPosition(float x, float y) {
    posX = x;
    posY = y;
    updateSpritePosition();
}