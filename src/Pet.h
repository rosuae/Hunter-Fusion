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

    Pet(const Pet &other)
        : Entity(other),
        target{other.target},
        isActivated{other.isActivated},
        activationRange{other.activationRange},
        minDistance{other.minDistance},
        frameWidth{other.frameWidth},
        frameHeight{other.frameHeight},
        animationTimer{other.animationTimer},
        animationSpeed{other.animationSpeed},
        currentFrame{other.currentFrame},
        isMoving{other.isMoving}
    {
        std::cout << "Copy constructor pet";
    }

    friend void swap(Pet &lhs, Pet &rhs) noexcept {
        using std::swap;
        swap(static_cast<Entity &>(lhs), static_cast<Entity &>(rhs));
        swap(lhs.target, rhs.target);
        swap(lhs.isActivated, rhs.isActivated);
        swap(lhs.activationRange, rhs.activationRange);
        swap(lhs.minDistance, rhs.minDistance);
    }

    Pet& operator=(const Pet& other) {
        if (this != &other) {
            const std::unique_ptr<Entity> clonedEntity = other.clone();
            auto* clonedPet = dynamic_cast<Pet*>(clonedEntity.get());
            using std::swap;
            swap(*this, *clonedPet);
        }
        return *this;
    }

    std::unique_ptr<Entity> clone() const override;
    ~Pet() override;
    void setOwner(Entity* owner) { target = owner; }
    void teleport(float x, float y);
};

#endif //OOP_PET_H