//
// Created by rosua on 12/5/2025.
//

#ifndef OOP_PORTAL_H
#define OOP_PORTAL_H

#include "Entity.h"
#include <list>
#include <SFML/Audio.hpp>

class Portal : public Entity {

    std::string nextMapFile;
    sf::Vector2f playerSpawnPosition;
    std::pair<std::string, sf::Vector2f> nextLocation;
    std::list<sf::Sound>* activeSounds;
    const sf::SoundBuffer* activationSound;

    sf::Vector2i frameSize;
    int currentFrame;
    int animationFrameCount;
    float animationTimer;
    float frameDuration;

    bool facingRight;
    bool isAnimating;
    bool isActive;

    sf::FloatRect doGetBounds() const override;
    void takeDamage(int damageAmount) override;
    void doBehavior(float deltaTime, const Map& map) override;
    void applyGravity(float deltaTime) override;

public:
    Portal(const std::string& n,
        float posx_, float posy_,
        sf::Texture& tex,
        std::string  nextMapFile_,
        bool facingRight_,
        sf::Vector2f playerSpawnPosition_,
        std::list<sf::Sound>& activeSounds_,
        const sf::SoundBuffer& activationSound_);
    ~Portal() override = default;

    Portal (const Portal &other) = default;

    friend void swap(Portal &lhs, Portal &rhs) noexcept {
        using std::swap;
        swap(static_cast<Entity &>(lhs), static_cast<Entity &>(rhs));
        swap(lhs.nextMapFile, rhs.nextMapFile);
        swap(lhs.playerSpawnPosition, rhs.playerSpawnPosition);
        swap(lhs.nextLocation, rhs.nextLocation);
        swap(lhs.activeSounds, rhs.activeSounds);
        swap(lhs.activationSound, rhs.activationSound);
        swap(lhs.frameSize, rhs.frameSize);
        swap(lhs.currentFrame, rhs.currentFrame);
        swap(lhs.animationFrameCount, rhs.animationFrameCount);
        swap(lhs.animationTimer, rhs.animationTimer);
        swap(lhs.frameDuration, rhs.frameDuration);
        swap(lhs.facingRight, rhs.facingRight);
        swap(lhs.isAnimating, rhs.isAnimating);
        swap(lhs.isActive, rhs.isActive);
    }

    Portal& operator=(Portal other) {
        Entity::operator=(other);
        swap(*this, other);
        return *this;
    }

    std::unique_ptr<Entity> clone() const override;

    [[nodiscard]]bool isObstacle() const override { return !isActive; }
    [[nodiscard]]const std::pair<std::string, sf::Vector2f>& teleportDestination() const;
};

#endif //OOP_PORTAL_H