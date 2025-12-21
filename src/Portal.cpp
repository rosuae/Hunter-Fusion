//
// Created by rosua on 12/5/2025.
//

#include "Portal.h"
#include <iostream>
#include <utility>

Portal::Portal(const std::string& n,
        const float posx_, const float posy_,
        sf::Texture& tex,
        std::string  nextMapFile_,
        const bool facingRight_,
        const sf::Vector2f playerSpawnPosition_,
        std::list<sf::Sound>& activeSounds_,
        const sf::SoundBuffer& activationSound_)
    : Entity(n, posx_, posy_, 0, 0, tex, 100, 288),
    nextMapFile{std::move(nextMapFile_)},
    playerSpawnPosition{playerSpawnPosition_},
    nextLocation{nextMapFile, playerSpawnPosition},
    activeSounds{&activeSounds_},
    activationSound{&activationSound_},
    frameSize{sf::Vector2i(190, 288)},
    currentFrame{0},
    animationFrameCount{5},
    animationTimer{0.0f},
    frameDuration{0.15f},
    facingRight{facingRight_},
    isAnimating{false},
    isActive{false}
{
    sprite.setTextureRect(sf::IntRect({0, 0}, {frameSize.x, frameSize.y}));

    sprite.setPosition(sf::Vector2f(posX, posY));

    if (facingRight) {
        sprite.setScale(sf::Vector2f(1.f, 1.f));
    } else {
        sprite.setOrigin({static_cast<float>(hitboxWidth), 0.f});
        sprite.setScale({-1.f, 1.f});
    }

    hitbox = sf::FloatRect({0, 0.f},
        {static_cast<float>(hitboxWidth),
        static_cast<float>(hitboxHeight)}
    );
}


Portal::Portal (const Portal& other)
    : Entity{other},
    activeSounds{other.activeSounds},
    activationSound{other.activationSound},
    currentFrame{other.currentFrame},
    animationFrameCount{other.animationFrameCount},
    animationTimer{other.animationTimer},
    frameDuration{other.frameDuration},
    facingRight(other.facingRight),
    isAnimating{other.isAnimating},
    isActive{other.isActive} {
    std::cout << "Portal copy constructor \n";
}

std::unique_ptr<Entity> Portal::clone() const {
    return std::make_unique<Portal>(*this);
}

Portal::~Portal() {std::cout<<"Portal destructor \n";}

sf::FloatRect Portal::doGetBounds() const {
    return sprite.getTransform().transformRect(hitbox);
}

void Portal::takeDamage(const int damageAmount) {
    health += damageAmount;

    if (!isActive && !isAnimating) {
        animationTimer = 0.0f;
        isAnimating = true;

        if (activeSounds && activationSound) {
            activeSounds->emplace_back(*activationSound);
            activeSounds->back().play();
        }
    }
}

void Portal::doBehavior(const float deltaTime, const Map&) {
    if (!isAnimating || isActive) {
        return;
    }

    animationTimer += deltaTime;

    if (animationTimer >= frameDuration) {
        animationTimer -= frameDuration;
        if (currentFrame < animationFrameCount - 1) {
            currentFrame++;

            int rectLeft = currentFrame * frameSize.y;
            int rectTop = 0;
            sprite.setTextureRect(sf::IntRect({rectLeft, rectTop}, {frameSize.x, frameSize.y}));}
        else {
            isActive = true;
            isAnimating = false;
        }
    }
}

void Portal::applyGravity(const float deltaTime) {
    posY += gravity * deltaTime;
}

const std::pair<std::string, sf::Vector2f>& Portal::teleportDestination() const{
        return nextLocation;
}