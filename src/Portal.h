//
// Created by rosua on 12/5/2025.
//

#ifndef OOP_PORTAL_H
#define OOP_PORTAL_H

#include "Entity.h"
#include <list>
#include <SFML/Audio.hpp>
#include <iostream>

class Portal : public Entity {

    std::string nextMapFile;
    sf::Vector2f playerSpawnPosition;
    std::list<sf::Sound>* activeSounds;
    const sf::SoundBuffer* activationSound;

    sf::Vector2i frameSize;
    int currentFrame;
    int animationFrameCount;
    float animationTimer;
    float frameDuration;

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
        sf::Vector2f playerSpawnPosition_,
        std::list<sf::Sound>& activeSounds_,
        const sf::SoundBuffer& activationSound_);

    friend void swap(Portal &lhs, Portal &rhs) noexcept {
        using std::swap;
        swap(static_cast<Entity &>(lhs), static_cast<Entity &>(rhs));
        swap(lhs.activeSounds, rhs.activeSounds);
        swap(lhs.activationSound, rhs.activationSound);
    }

    Portal& operator= (const Portal& other) {
        if (this != &other) {
            auto copie = other.clone();
            using std::swap;
            swap(*this, *copie);
        }
        return *this;
    }

    Portal (const Portal& other);
    bool isObstacle() const override {return !isOpen();}
    std::unique_ptr<Entity> clone() const override;
    ~Portal() override;
    void flipHorizontally();
    [[nodiscard]]sf::Vector2f getNextPlayerSpawn() const;
    [[nodiscard]]const std::string& getNextMapFile() const;
    [[nodiscard]] bool isOpen() const;
};

#endif //OOP_PORTAL_H