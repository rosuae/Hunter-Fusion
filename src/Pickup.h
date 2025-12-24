//
// Created by rosua on 12/23/2025.
//

#ifndef OOP_PICKUP_H
#define OOP_PICKUP_H

#include "Entity.h"
#include <SFML/Audio.hpp>
#include <list>
#include <memory>

class Player;
class Map;
class ResourceManager;

class Pickup : public Entity {
protected:
    bool isGrounded;
    float floatTimer;
    float basePosY;
    std::list<sf::Sound>* activeSounds;
    const sf::SoundBuffer* pickupSound;

public:
    Pickup(std::string n, const float x, const float y, sf::Texture& tex, std::list<sf::Sound>& activeSounds_, const sf::SoundBuffer& pickupSound_)
        : Entity(std::move(n), x, y, 0.0f, 400.0f, tex, 20, 20),
        isGrounded{false},
        floatTimer{0.f},
        basePosY{y},
        activeSounds{&activeSounds_},
        pickupSound{&pickupSound_} {}

    virtual void apply(Player& player) = 0;
    void takeDamage(const int damageAmount) override { health += damageAmount; }
    void applyGravity(const float deltaTime) override { posY += gravity * deltaTime; }
    sf::FloatRect doGetBounds() const override { return hitbox; }

    void doBehavior(float deltaTime, const Map& map) override;

    ~Pickup() override = default;
};

class AmmoPickup : public Pickup {
    const int AMMO_REWARD = 6;
public:
    AmmoPickup(float x, float y, sf::Texture& tex, std::list<sf::Sound>& activeSounds_, const sf::SoundBuffer& pickupSound_);

    std::unique_ptr<Entity> clone() const override;
    void apply(Player& player) override;
};

enum class PickupType { Ammo, Health };

class PickupFactory {
public:
    static std::unique_ptr<Entity> create(PickupType type, float x, float y, ResourceManager &resManager, std::list<sf::Sound> &playingSounds);
};

#endif //OOP_PICKUP_H