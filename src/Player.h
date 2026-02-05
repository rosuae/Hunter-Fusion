#ifndef PLAYER_H
#define PLAYER_H

#include "Entity.h"
#include <SFML/Audio.hpp>
#include <list>
#include <optional>
#include <iostream>

#include "Weapon.h"

class Map;
class Pet;
class Weapon;

class Player : public Entity {
    static int s_highScore;
    sf::Vector2f velocity;
    float maxJump;
    float jumpCooldown;

    std::unique_ptr<Weapon> weapon;
    Entity* m_pendingPet = nullptr;

    int totalBounty = 0;
    int enemiesDefeated = 0;

    float animationTimer;
    float idleTimer;
    float frameDuration;
    float shootingTimer;
    float damageEffectTimer;
    float dodgeTimer;
    float dodgeCooldownTimer;
    float storedDodgeDir;

    bool isRunning;
    bool isJumping;
    bool isDodging;
    bool facingRight;
    bool facingUp;
    bool isHit;
    bool wasRPressedLastFrame;
    bool isCrouching;

    std::optional<std::pair<std::string, sf::Vector2f>> pendingTeleport;

    sf::Vector2i m_frameSize;
    int currentFrame;
    int animationRow;
    int animationStartIndex;
    int animationFrameCount;
    sf::RectangleShape damageOverlay;

    std::list<sf::Sound>& activeSounds;
    sf::Sound jumpSound;
    sf::Sound deathSound;
    sf::Sound dodgeSound;

    static constexpr float shootingDuration = 0.3f;
    static constexpr float damageEffectDuration = 0.3f;

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

    static constexpr float DODGE_DURATION = 0.6f;
    static constexpr float DODGE_COOLDOWN_TIME = 0.8f;
    static constexpr float DODGE_SPEED = 1000.f;

    static constexpr int ANIM_ROW_IDLE = 0;
    static constexpr int ANIM_ROW_SHOOT = 1;
    static constexpr int ANIM_ROW_UP = 2;
    static constexpr int ANIM_COLS_CROUCH = 4;

    struct tierUpgrade {
        int scoreThreshold;
        const sf::Texture* skinTexture;
        const sf::SoundBuffer* sound;
        bool unlocked;
    };

    std::vector<tierUpgrade> m_availableSkins;

    sf::FloatRect doGetBounds() const override;
    void takeDamage(int damageAmount) override;
    void doBehavior(float deltaTime, const Map&) override;
    void applyGravity(float deltaTime) override;

    void handleInput(float deltaTime, const Map& map);
    void handleMovementInput(float deltaTime);
    void handleJumpInput(const Map& map);
    void handleVariableJumpHeight(float deltaTime);
    void handleCrouchInput(const Map& map);
    void startDodge();
    void handleShootingInput(const Map& map);

    void resolveCollisionX(const Map& map, float lastPosX);
    void resolveCollisionY(const Map& map, float lastPosY);
    bool checkCeilingCollision(const Map& map) const;

    void checkTierUpgrade();
    void changeSkin(const sf::Texture& newTexture);
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
        const sf::SoundBuffer& deathSound_,
        const sf::SoundBuffer& dodgeSound_,
        std::unique_ptr<Weapon> startingWeapon);

    Player(const Player &other)
        : Entity(other),
          velocity(other.velocity),
          maxJump(other.maxJump),
          jumpCooldown(other.jumpCooldown),
          weapon(other.weapon ? other.weapon->clone() : nullptr),
          totalBounty(other.totalBounty),
          enemiesDefeated(other.enemiesDefeated),
          animationTimer(other.animationTimer),
          idleTimer(other.idleTimer),
          frameDuration(other.frameDuration),
          shootingTimer(other.shootingTimer),
          damageEffectTimer(other.damageEffectTimer),
          dodgeTimer(other.dodgeTimer),
          dodgeCooldownTimer(other.dodgeCooldownTimer),
          storedDodgeDir(other.storedDodgeDir),
          isRunning(other.isRunning),
          isJumping(other.isJumping),
          isDodging(other.isDodging),
          facingRight(other.facingRight),
          facingUp(other.facingUp),
          isHit(other.isHit),
          wasRPressedLastFrame(other.wasRPressedLastFrame),
          isCrouching(other.isCrouching),
          m_frameSize(other.m_frameSize),
          currentFrame(other.currentFrame),
          animationRow(other.animationRow),
          animationStartIndex(other.animationStartIndex),
          animationFrameCount(other.animationFrameCount),
          damageOverlay(other.damageOverlay),
          activeSounds(other.activeSounds),
          jumpSound(other.jumpSound),
          deathSound(other.deathSound),
          dodgeSound(other.dodgeSound),
          m_availableSkins(other.m_availableSkins) {
    }

    friend void swap(Player &lhs, Player &rhs) noexcept {
        using std::swap;
        swap(static_cast<Entity &>(lhs), static_cast<Entity &>(rhs));
        swap(lhs.velocity, rhs.velocity);
        swap(lhs.maxJump, rhs.maxJump);
        swap(lhs.jumpCooldown, rhs.jumpCooldown);
        swap(lhs.weapon, rhs.weapon);
        swap(lhs.totalBounty, rhs.totalBounty);
        swap(lhs.enemiesDefeated, rhs.enemiesDefeated);
        swap(lhs.animationTimer, rhs.animationTimer);
        swap(lhs.idleTimer, rhs.idleTimer);
        swap(lhs.frameDuration, rhs.frameDuration);
        swap(lhs.shootingTimer, rhs.shootingTimer);
        swap(lhs.damageEffectTimer, rhs.damageEffectTimer);
        swap(lhs.isDodging, rhs.isDodging);
        swap(lhs.dodgeTimer, rhs.dodgeTimer);
        swap(lhs.dodgeCooldownTimer, rhs.dodgeCooldownTimer);
        swap(lhs.storedDodgeDir, rhs.storedDodgeDir);
        swap(lhs.isRunning, rhs.isRunning);
        swap(lhs.isJumping, rhs.isJumping);
        swap(lhs.facingRight, rhs.facingRight);
        swap(lhs.facingUp, rhs.facingUp);
        swap(lhs.isHit, rhs.isHit);
        swap(lhs.wasRPressedLastFrame, rhs.wasRPressedLastFrame);
        swap(lhs.isCrouching, rhs.isCrouching);
        swap(lhs.m_frameSize, rhs.m_frameSize);
        swap(lhs.currentFrame, rhs.currentFrame);
        swap(lhs.animationRow, rhs.animationRow);
        swap(lhs.animationStartIndex, rhs.animationStartIndex);
        swap(lhs.animationFrameCount, rhs.animationFrameCount);
        swap(lhs.damageOverlay, rhs.damageOverlay);
        swap(lhs.activeSounds, rhs.activeSounds);
        swap(lhs.jumpSound, rhs.jumpSound);
        swap(lhs.deathSound, rhs.deathSound);
        swap(lhs.dodgeSound, rhs.dodgeSound);
        swap(lhs.m_availableSkins, rhs.m_availableSkins);
    }

    Player& operator=(Player other) {
        Entity::operator=(other);
        swap(*this, other);
        return *this;
    }

    std::unique_ptr<Entity> clone() const override{
        return std::make_unique<Player>(*this);
    }

    ~Player() override = default;
    void spawnAt(float x, float y) override;
    void resurrect();
    void draw(sf::RenderWindow& window) const override;
    void fire(const sf::Vector2f& direction);
    void processKill(int scoreReward);
    void collectPet(Pet* pet);
    void addSkinUnlock(int score, const sf::Texture& skinTexture_, const sf::SoundBuffer& sound);
    void checkProjectileCollisions(const std::vector<std::unique_ptr<Entity>>& targets) const;
    void resetWeaponProjectiles() const;
    void scheduleTeleport(std::string mapName, sf::Vector2f spawnPos) { pendingTeleport = std::make_pair(mapName, spawnPos); }
    bool pickupAmmo(int amount) const;

    sf::Vector2f getWeaponTipPos() const;
    Entity* takePendingPet();
    std::optional<std::pair<std::string, sf::Vector2f>> consumeTeleportRequest();
    [[nodiscard]]std::pair<int, int> combatStats() const { return {enemiesDefeated, totalBounty}; }
    [[nodiscard]]int getHealth() const { return health; }
    [[nodiscard]]const Weapon* getWeapon() const { return weapon.get(); }

    static int getHighScore();
    static void loadHighScore();
    static void saveHighScore();
};

#endif