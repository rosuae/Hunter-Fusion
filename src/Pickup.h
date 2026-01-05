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

    void takeDamage(const int damageAmount) override { health += damageAmount; }
    void applyGravity(const float deltaTime) override { posY += gravity * deltaTime; }
    sf::FloatRect doGetBounds() const override { return hitbox; }
    void doBehavior(float deltaTime, const Map& map) override;
public:
    Pickup(std::string n, const float x, const float y, sf::Texture& tex, std::list<sf::Sound>& activeSounds_, const sf::SoundBuffer& pickupSound_)
        : Entity(std::move(n), x, y, 0.0f, 400.0f, tex, 20, 20),
        isGrounded{false},
        floatTimer{0.f},
        basePosY{y},
        activeSounds{&activeSounds_},
        pickupSound{&pickupSound_} {}

    ~Pickup() override = default;
    Pickup(const Pickup &other) = default;

    friend void swap(Pickup &lhs, Pickup &rhs) noexcept {
        using std::swap;
        swap(static_cast<Entity &>(lhs), static_cast<Entity &>(rhs));
        swap(lhs.isGrounded, rhs.isGrounded);
        swap(lhs.floatTimer, rhs.floatTimer);
        swap(lhs.basePosY, rhs.basePosY);
        swap(lhs.activeSounds, rhs.activeSounds);
        swap(lhs.pickupSound, rhs.pickupSound);
    }

    virtual void apply(Player& player) = 0;
    void interactWithPlayer(Player &player) override;
    void onCollision(Player &player) override { this->apply(player); }
    int getCollisionPriority() const override { return PRIORITY_PICKUP; }
};

//
// Ammo Pickup
//

class AmmoPickup : public Pickup {
    int AMMO_REWARD = 6;
public:
    AmmoPickup(float x, float y, sf::Texture& tex, std::list<sf::Sound>& activeSounds_, const sf::SoundBuffer& pickupSound_);

    ~AmmoPickup() override = default;
    AmmoPickup(const AmmoPickup &other) = default;

    friend void swap(AmmoPickup &lhs, AmmoPickup &rhs) noexcept {
        using std::swap;
        swap(static_cast<Pickup &>(lhs), static_cast<Pickup &>(rhs));
        swap(lhs.AMMO_REWARD, rhs.AMMO_REWARD);
    }

    AmmoPickup& operator=(AmmoPickup other) {
        Entity::operator=(other);
        swap(*this, other);
        return *this;
    }

    std::unique_ptr<Entity> clone() const override {
        return std::make_unique<AmmoPickup>(*this);
    }

    void apply(Player& player) override;
};

//
//  Pickup factory
//

enum class PickupType { Ammo, Health };

class PickupFactory {
public:
    static std::unique_ptr<Entity> create(PickupType type, float x, float y, ResourceManager &resManager, std::list<sf::Sound> &playingSounds);
};

#endif //OOP_PICKUP_H