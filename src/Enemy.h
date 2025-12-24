#ifndef OOP_ENEMY_H
#define OOP_ENEMY_H

#include "Weapon.h"
#include "Entity.h"
#include "EnemyFactory.h"

class Map;
class Player;

enum class EnemyState {
    Patrolling,
    Chasing,
    Attacking
};

class Enemy : public Entity{
    int damage;
    int bountyScore;
    static int activeEnemyCount;
    Entity* target;
    bool isMoving;
    bool canDealDamage;
    EnemyState state;
    float detectionRange;
    float attackCooldown;
    float currentAttackTimer;
    float aggroTimer;

    float patrolTimer;
    float patrolDuration;
    float patrolDirection;

    sf::Vector2i frameSize;
    int currentFrame;
    float animationTimer;
    float frameDuration;
    int animationFrameCount;

    const sf::Texture* alertTexture;
    sf::Sprite exclamationSprite;
    sf::Vector2i alertFrameSize;

    float alertAnimTimer;
    bool alertActive;

    std::list<sf::Sound>* activeSounds;
    const sf::SoundBuffer* hitSound;
    const sf::SoundBuffer* deathSound;

    sf::FloatRect doGetBounds() const override;
    void applyGravity(float deltaTime) override;

    void takeDamage(int damageAmount) override;
    void doBehavior(float deltaTime, const Map& map) override;
    void updateAI(float deltaTime);
    void updatePatrol(float deltaTime);
    void updateChase(float deltaTime);
    void updateAttack(float deltaTime);
    void updatePhysics(float deltaTime, const Map& map);
    void updateAnimation(float deltaTime);
    void draw(sf::RenderWindow& window) const override;

    friend class EnemyFactory;

    Enemy(const std::string& n,
        int damage_, int bountyScore_,
        float posx_, float posy_,
        sf::Texture& tex, const sf::Texture& alertTex,
        std::list<sf::Sound>& activeSounds_,
        const sf::SoundBuffer& hitSound_,
        const sf::SoundBuffer& deathSound_,
        Entity* target_);

public:
    friend std::ostream& operator<< (std::ostream& out, const Enemy& e) {
        out << " Nume inamic: " << e.name << " Pos X: " << e.posX << " Pos Y: " << e.posY << " Viata inamic: " << e.health << " Viteza imanic: " << e.speed;
        return out;
    }

    ~Enemy() override{
            activeEnemyCount--;
        }

    Enemy(const Enemy &other)
        : Entity(other),
          damage(other.damage),
          bountyScore(other.bountyScore),
          target(other.target),
          isMoving(other.isMoving),
          canDealDamage(other.canDealDamage),
          state(other.state),
          detectionRange(other.detectionRange),
          attackCooldown(other.attackCooldown),
          currentAttackTimer(other.currentAttackTimer),
          aggroTimer(other.aggroTimer),
          patrolTimer(other.patrolTimer),
          patrolDuration(other.patrolDuration),
          patrolDirection(other.patrolDirection),
          frameSize(other.frameSize),
          currentFrame(other.currentFrame),
          animationTimer(other.animationTimer),
          frameDuration(other.frameDuration),
          animationFrameCount(other.animationFrameCount),
          alertTexture(other.alertTexture),
          exclamationSprite(other.exclamationSprite),
          alertFrameSize(other.alertFrameSize),
          alertAnimTimer(other.alertAnimTimer),
          alertActive(other.alertActive),
          activeSounds(other.activeSounds),
          hitSound(other.hitSound),
          deathSound(other.deathSound) {
        activeEnemyCount++;
    }

    friend void swap(Enemy &lhs, Enemy &rhs) noexcept {
        using std::swap;
        swap(static_cast<Entity &>(lhs), static_cast<Entity &>(rhs));
        swap(lhs.damage, rhs.damage);
        swap(lhs.bountyScore, rhs.bountyScore);
        swap(lhs.target, rhs.target);
        swap(lhs.isMoving, rhs.isMoving);
        swap(lhs.canDealDamage, rhs.canDealDamage);
        swap(lhs.state, rhs.state);
        swap(lhs.detectionRange, rhs.detectionRange);
        swap(lhs.attackCooldown, rhs.attackCooldown);
        swap(lhs.currentAttackTimer, rhs.currentAttackTimer);
        swap(lhs.aggroTimer, rhs.aggroTimer);
        swap(lhs.patrolTimer, rhs.patrolTimer);
        swap(lhs.patrolDuration, rhs.patrolDuration);
        swap(lhs.patrolDirection, rhs.patrolDirection);
        swap(lhs.frameSize, rhs.frameSize);
        swap(lhs.currentFrame, rhs.currentFrame);
        swap(lhs.animationTimer, rhs.animationTimer);
        swap(lhs.frameDuration, rhs.frameDuration);
        swap(lhs.animationFrameCount, rhs.animationFrameCount);
        swap(lhs.alertTexture, rhs.alertTexture);
        swap(lhs.exclamationSprite, rhs.exclamationSprite);
        swap(lhs.alertFrameSize, rhs.alertFrameSize);
        swap(lhs.alertAnimTimer, rhs.alertAnimTimer);
        swap(lhs.alertActive, rhs.alertActive);
        swap(lhs.activeSounds, rhs.activeSounds);
        swap(lhs.hitSound, rhs.hitSound);
        swap(lhs.deathSound, rhs.deathSound);
    }

    Enemy & operator=(Enemy other) {
        Entity::operator=(other);
        swap(*this, other);
        return *this;
    }

    std::unique_ptr<Entity> clone() const override{
        return std::make_unique<Enemy>(*this);
    }

    static int getActiveEnemyCount();
    void grantReward(Player& player) const;
    int attackPlayer();
};

#endif