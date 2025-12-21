//
// Created by rosua on 12/16/2025.
//

#include "Pet.h"
#include <cmath>
#include <utility>

#include "Map.h"

Pet::Pet(std::string n, const float x, const float y, sf::Texture& tex, const Entity* playerTarget)
    : Entity(std::move(n), x, y, 350.0f, 600.0f, tex, static_cast<int>(tex.getSize().x), static_cast<int>(tex.getSize().y)),
    target{playerTarget},
    isActivated{false},
    activationRange{100.0f},
    minDistance{60.0f},
    animationTimer{0.0f},
    currentFrame{0},
    isMoving{false}
{
    frameWidth = static_cast<int>(tex.getSize().x) / 7;
    frameHeight = static_cast<int>(tex.getSize().y);

    hitboxWidth = frameWidth / 2;
    hitboxHeight = frameHeight / 2;

    sprite.setTextureRect(sf::IntRect({0, 0}, {frameWidth, frameHeight}));

    sprite.setOrigin({
        static_cast<float>(frameWidth) / 2.0f,
        static_cast<float>(frameHeight) - 15.f
    });
}

float Pet::getDistanceToTarget() const {
    if (!target) return 99999.0f;

    const sf::Vector2f targetPos = target->getPos();

    const float dx = targetPos.x - posX;
    const float dy = targetPos.y - posY;

    return std::sqrt(dx * dx + dy * dy);
}

void Pet::doBehavior(const float deltaTime, const Map& map) {
    if (!target) return;

    isMoving = false;
    const float dist = getDistanceToTarget();

    if (!isActivated) {
        if (dist < activationRange) {
            isActivated = true;
        }
    }

    if (isActivated) {
        if (dist > 1200.0f) {
            const sf::Vector2f targetPos = target->getPos();
            teleport(targetPos.x, targetPos.y - 50.0f);
        }
        else if (dist > minDistance) {
            const sf::Vector2f targetPos = target->getPos();
            const float dx = targetPos.x - posX;

            float dirX = 0.0f;
            if (std::abs(dx) > 5.0f) {
                dirX = dx > 0 ? 1.0f : -1.0f;
            }

            if (dirX != 0.0f) {
                isMoving = true;
            }

            const float oldX = posX;
            posX += dirX * speed * deltaTime;
            updateHitbox();

            if (map.isWall(hitbox, true)) {
                posX = oldX;
                updateHitbox();

                posY -= Map::getTileSize() - 10.f;
            }

            if (dirX < 0) {
                sprite.setScale({-1.0f, 1.0f});
            } else {
                sprite.setScale({1.0f, 1.0f});
            }
        }
    }

    applyGravity(deltaTime);
    updateHitbox();

    if (map.isWall(hitbox, true)) {
        const float tileSize = Map::getTileSize();
        posY = std::floor(posY / tileSize) * tileSize;

        updateHitbox();
    }

    updateAnimation(deltaTime);
}

void Pet::updateAnimation(const float deltaTime) {
    animationTimer += deltaTime;

    float currentSpeedLimit = isMoving ? animationSpeed : animationSpeed * 4.0f;

    if (animationTimer >= currentSpeedLimit) {
        animationTimer = 0.0f;
        currentFrame++;

        if (isMoving) {
            if (currentFrame < 2 || currentFrame > 6) {
                currentFrame = 2;
            }
        } else {
            if (currentFrame > 1) {
                currentFrame = 0;
            }
        }

        int rectLeft = currentFrame * frameWidth;
        sprite.setTextureRect(sf::IntRect({rectLeft, 0}, {frameWidth, frameHeight}));
    }
}

void Pet::applyGravity(const float deltaTime) {
    posY += gravity * deltaTime;
}

sf::FloatRect Pet::doGetBounds() const {
    return hitbox;
}

void Pet::takeDamage(const int damageAmount) {
    health += damageAmount;
    std::cout << "invincible \n";
}

std::unique_ptr<Entity> Pet::clone() const{
    return std::make_unique<Pet>(*this);
}

Pet::~Pet() { std::cout << "Pet destructor"; }

void Pet::teleport(const float x, const float y) {
    setPosition(x, y);
    updateHitbox();
}