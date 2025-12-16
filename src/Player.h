#ifndef PLAYER_H
#define PLAYER_H

#include "Entity.h"
#include <SFML/Audio.hpp>
#include <list>
#include <iostream>

#include "Weapon.h"

class Map;
class Weapon;

class Player : public Entity {
    float velocity;
    float maxJump;
    float jumpCooldown;

    std::unique_ptr<Weapon> weapon;

    float animationTimer;
    float frameDuration;
    float shootingTimer;
    static constexpr float shootingDuration = 0.3f;
    float damageEffectTimer;

    bool isRunning;
    bool isJumping;
    bool facingRight;
    bool facingUp;
    bool isHit;
    bool wasRPressedLastFrame;
    bool isCrouching;

    sf::Vector2i frameSize;
    int currentFrame;
    int animationRow;
    int animationStartIndex;
    int animationFrameCount;
    sf::RectangleShape damageOverlay;

    std::list<sf::Sound>& activeSounds;
    sf::Sound jumpSound;

    sf::FloatRect doGetBounds() const override;
    void takeDamage(int damageAmount) override;
    void doBehavior(float deltaTime, const Map&) override;
    void applyGravity(float deltaTime) override;
    void updateSpriteDirection();
    void updateAnimation(float deltaTime);
    void handleInput (float deltaTime, const Map& map);
    void updateDamageEffect(float deltaTime);

public:
    Player(const std::string& n, sf::Texture& tex,
        float posx_, float posy_,
        std::list<sf::Sound>& activeSounds_,
        const sf::SoundBuffer& jumpSound_,
        std::unique_ptr<Weapon> startingWeapon);

    Player& operator=(const Player& other) {
        if (this != &other){
            const auto copie = other.clone();
            using std::swap;
            swap(*this, *copie);
            std::cout << "Copy and swap\n";
        }
        return *this;
    }

    Player(const Player &other) :
    Entity(other),
    velocity(other.velocity),
    maxJump(other.maxJump),
    jumpCooldown(other.jumpCooldown),
    weapon(other.weapon ? other.weapon->clone() : nullptr),
    animationTimer(other.animationTimer),
    frameDuration(other.frameDuration),
    shootingTimer(other.shootingTimer),
    damageEffectTimer(other.damageEffectTimer),
    isRunning(other.isRunning),
    isJumping(other.isJumping),
    facingRight(other.facingRight),
    facingUp(other.facingUp),
    isHit(other.isHit),
    wasRPressedLastFrame(other.wasRPressedLastFrame),
    isCrouching{other.isCrouching},
    frameSize(other.frameSize),
    currentFrame(other.currentFrame),
    animationRow(other.animationRow),
    animationStartIndex(other.animationStartIndex),
    animationFrameCount(other.animationFrameCount),
    damageOverlay(other.damageOverlay),
    activeSounds(other.activeSounds),
    jumpSound(other.jumpSound) {
    }

    friend void swap(Player &lhs, Player &rhs) noexcept {
        using std::swap;
        swap(static_cast<Entity &>(lhs), static_cast<Entity &>(rhs));
        swap(lhs.velocity, rhs.velocity);
        swap(lhs.maxJump, rhs.maxJump);
        swap(lhs.weapon, rhs.weapon);
        swap(lhs.animationTimer, rhs.animationTimer);
        swap(lhs.frameDuration, rhs.frameDuration);
        swap(lhs.shootingTimer, rhs.shootingTimer);
        swap(lhs.damageEffectTimer, rhs.damageEffectTimer);
        swap(lhs.isRunning, rhs.isRunning);
        swap(lhs.isJumping, rhs.isJumping);
        swap(lhs.facingRight, rhs.facingRight);
        swap(lhs.facingUp, rhs.facingUp);
        swap(lhs.isHit, rhs.isHit);
        swap(lhs.wasRPressedLastFrame, rhs.wasRPressedLastFrame);
        swap(lhs.isCrouching, rhs.isCrouching);
        swap(lhs.frameSize, rhs.frameSize);
        swap(lhs.currentFrame, rhs.currentFrame);
        swap(lhs.animationRow, rhs.animationRow);
        swap(lhs.animationStartIndex, rhs.animationStartIndex);
        swap(lhs.animationFrameCount, rhs.animationFrameCount);
        swap(lhs.damageOverlay, rhs.damageOverlay);
        swap(lhs.activeSounds, rhs.activeSounds);
        swap(lhs.jumpSound, rhs.jumpSound);
    }

    std::unique_ptr<Entity> clone() const override;
    ~Player() override;

    void spawn(float x, float y);
    void resurrect();
    void draw(sf::RenderWindow& window) const override;
    void fire(const sf::Vector2f& direction);
    void checkProjectileCollisions(const std::vector<std::unique_ptr<Entity>>& targets) const;
    void resetWeaponProjectiles() const;

    sf::Vector2f getWeaponTipPos() const;
    bool hitAffected() const;
    [[nodiscard]]int getHealth() const { return health; }
    [[nodiscard]]const Weapon* getWeapon() const { return weapon.get(); }
};

#endif