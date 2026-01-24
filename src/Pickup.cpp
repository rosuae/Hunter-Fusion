//
// Created by rosua on 12/23/2025.
//

#include "Pickup.h"
#include "Player.h"
#include "ResourceManager.h"
#include "Map.h"
#include <cmath>

void Pickup::doBehavior(const float deltaTime, const Map& map) {
    if (!isGrounded) {
        const float lastY = posY;

        applyGravity(deltaTime);

        if (posY != lastY) {
            updateHitbox();

            if (map.isWall(hitbox, true)) {
                posY = lastY;
                updateHitbox();

                isGrounded = true;
                basePosY = posY;
                floatTimer = 0.0f;
            }
        }
    }
    else {
        floatTimer += deltaTime;

        constexpr float amplitude = 10.0f;
        constexpr float animSpeed = 2.0f;
        constexpr float groundOffsetY = 10;

        posY = basePosY + std::cos(floatTimer * animSpeed) * amplitude - groundOffsetY;

        updateHitbox();
    }
    updateSpritePosition();
}

AmmoPickup::AmmoPickup(const float x, const float y, sf::Texture& tex, std::list<sf::Sound>& activeSounds_, const sf::SoundBuffer& pickupSound_)
    : Pickup("Ammo", x, y, tex, activeSounds_, pickupSound_) {
    hitboxWidth = 64;
    hitboxHeight = 64;

    const sf::Vector2u texSize = tex.getSize();
    sprite.setOrigin({static_cast<float>(texSize.x) / 2.0f, static_cast<float>(texSize.y)});
    updateHitbox();
}

void AmmoPickup::apply(Player& player) {
    if (!isAlive()) return;
    if (player.pickupAmmo(AMMO_REWARD)) {
        activeSounds->emplace_back(*pickupSound);
        activeSounds->back().play();
        this->health = 0;
        alive = false;
    }
}

void Pickup::onCollision(Player &player) {
    if (!isAlive()) return;
    this->apply(player);
}

std::unique_ptr<Entity> PickupFactory::create(const PickupType type, float x, float y, ResourceManager &resManager, std::list<sf::Sound> &playingSounds) {
    switch (type) {
        case PickupType::Ammo:
            return std::make_unique<AmmoPickup>(x, y, resManager.textures().get("ammo.png"), playingSounds, resManager.sounds().get("pickupammo.wav"));
        default:
            return nullptr;
    }
}