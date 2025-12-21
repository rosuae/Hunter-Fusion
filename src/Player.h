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
    sf::Vector2f velocity;
    float maxJump;
    float jumpCooldown;

    std::unique_ptr<Weapon> weapon;

    float animationTimer;
    float frameDuration;
    float shootingTimer;
    static constexpr float shootingDuration = 0.3f;
    float damageEffectTimer;
    static constexpr float damageEffectDuration = 0.3f;

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

    static constexpr float DEF_SPEED = 800.f;
    static constexpr float DEF_GRAVITY = 2500.f;
    static constexpr float DEF_JUMP_FORCE = -1450.f;

    static constexpr float ACCELERATION = 8000.f;
    static constexpr float DECELERATION = 4000.f;
    static constexpr float JUMP_CUT_MULTIPLIER = 0.4f;
    static constexpr float JUMP_COOLDOWN_TIME = 0.2f;

    static constexpr float HITBOX_WIDTH = 80.f;
    static constexpr float HITBOX_HEIGHT_STANDING = 160.f;
    static constexpr float HITBOX_HEIGHT_CROUCHING = 110.f;
    static constexpr float CEILING_CHECK_OFFSET = 20.f;
    static constexpr float JUMP_BUFFER_X = 10.f;
    static constexpr float GROUND_CHECK_HEIGHT = 10.f;

    static constexpr sf::Vector2i FRAME_SIZE = {224, 222};
    static constexpr float SPRITE_OFFSET_Y = -7.f;

    static constexpr int ANIM_ROW_IDLE = 0;
    static constexpr int ANIM_ROW_SHOOT = 1;
    static constexpr int ANIM_ROW_UP = 2;
    static constexpr int ANIM_COLS_CROUCH = 4;

    sf::FloatRect doGetBounds() const override;
    void takeDamage(int damageAmount) override;
    void doBehavior(float deltaTime, const Map&) override;
    void applyGravity(float deltaTime) override;

    void handleInput(float deltaTime, const Map& map);
    void handleMovementInput(float deltaTime);
    void handleJumpInput(const Map& map);
    void handleVariableJumpHeight(float deltaTime);
    void handleCrouchInput(const Map& map);
    void handleShootingInput(const Map& map);

    void resolveCollisionX(const Map& map, float lastPosX);
    void resolveCollisionY(const Map& map, float lastPosY);
    bool checkCeilingCollision(const Map& map) const;

    void updateSpriteDirection();
    void updateAnimation(float deltaTime);
    sf::IntRect calculateAnimationRect();
    void applySpriteOriginCorrection();
    void updateDamageEffect(float deltaTime);

public:
    Player(const std::string& n, sf::Texture& tex,
        float posx_, float posy_,
        std::list<sf::Sound>& activeSounds_,
        const sf::SoundBuffer& jumpSound_,
        std::unique_ptr<Weapon> startingWeapon);

    Player& operator=(const Player& other) {
        if (this != &other) {
            const std::unique_ptr<Entity> clonedEntity = other.clone();
            auto* clonedPlayer = dynamic_cast<Player*>(clonedEntity.get());
            using std::swap;
            swap(*this, *clonedPlayer);
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
    [[nodiscard]]int getHealth() const { return health; }
    [[nodiscard]]const Weapon* getWeapon() const { return weapon.get(); }
};

#endif