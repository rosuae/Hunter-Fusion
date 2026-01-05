//
// Created by rosua on 12/16/2025.
//

#ifndef OOP_PET_H
#define OOP_PET_H

#include "Entity.h"
#include <iostream>

class Pet : public Entity {
    Entity* target;
    bool isActivated;
    float activationRange;
    float minDistance;

    int frameWidth;
    int frameHeight;
    float animationTimer;
    float animationSpeed = 0.15f;
    int currentFrame;
    bool isMoving;

    float getDistanceToTarget() const;
    void updateAI(float deltaTime, const Map& map);
    void moveTowardsTarget( float deltaTime, const Map& map);
    void updateMapPhysics(float deltaTime, const Map& map);
    void updateAnimation(float deltaTime);

    sf::FloatRect doGetBounds() const override;
    void takeDamage(int damageAmount) override;
    void doBehavior(float deltaTime, const Map& map) override;
    void applyGravity(float deltaTime) override;

public:
    Pet(std::string n, float x, float y, sf::Texture& tex, Entity* playerTarget);
    ~Pet() override = default;

    Pet(const Pet &other) = default;

    friend void swap(Pet &lhs, Pet &rhs) noexcept {
        using std::swap;
        swap(static_cast<Entity &>(lhs), static_cast<Entity &>(rhs));
        swap(lhs.target, rhs.target);
        swap(lhs.isActivated, rhs.isActivated);
        swap(lhs.activationRange, rhs.activationRange);
        swap(lhs.minDistance, rhs.minDistance);
        swap(lhs.frameWidth, rhs.frameWidth);
        swap(lhs.frameHeight, rhs.frameHeight);
        swap(lhs.animationTimer, rhs.animationTimer);
        swap(lhs.animationSpeed, rhs.animationSpeed);
        swap(lhs.currentFrame, rhs.currentFrame);
        swap(lhs.isMoving, rhs.isMoving);
    }

    Pet & operator=(Pet other) {
        Entity::operator=(other);
        swap(*this, other);
        return *this;
    }

    std::unique_ptr<Entity> clone() const override;

    void setOwner(Entity* owner) { target = owner; }
    void onCollision(Player &player) override;
    void spawnAt(float x, float y) override;
};

#endif //OOP_PET_H