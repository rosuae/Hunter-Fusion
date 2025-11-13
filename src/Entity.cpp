#include "Entity.h"
#include <iostream>

Entity::Entity(std::string n, float x, float y, float spd, float grav, sf::Texture& tex)
    : name{std::move(n)},
      health{100},
      posX{x},
      posY{y},
      speed{spd},
      gravity{grav},
      alive{true},
      texture{tex},
      sprite{texture} {
}

Entity::~Entity() {
    std::cout << "S a apelat destructor Entity\n";
}

Entity::Entity (const Entity& other):
name{other.name},
health{other.health},
posX{other.posX},
posY{other.posY},
speed{other.speed},
gravity{other.gravity},
alive{other.alive},
texture{other.texture},
sprite{other.sprite}{
}

void Entity::applyGravity(float deltaTime) {
    posY += gravity * deltaTime;
}

void Entity::updateSpritePosition() {
    sprite.setPosition(sf::Vector2f(posX, posY));
}

void Entity::checkDeath() {
    if (health <= 0) {
        health = 0;
        alive = false;
    }
}

sf::Vector2f Entity::getPos() const {
    return {posX, posY};
}

bool Entity::isAlive() const {
    return alive;
}

void Entity::setPosition(float x, float y) {
    posX = x;
    posY = y;
    updateSpritePosition();
}