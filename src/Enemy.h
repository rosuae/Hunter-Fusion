#ifndef OOP_ENEMY_H
#define OOP_ENEMY_H

#include "Weapon.h"
#include "Entity.h"
#include "EnemyFactory.h"

class Map;

enum class EnemyState {
    Patrolling,
    Chasing,
    Attacking
};

class Enemy : public Entity{
    int damage;
    static int activeEnemyCount;
    Entity* target;
    bool isMoving;
    bool canDealDamage;
    EnemyState state;
    float detectionRange;
    float attackRange;
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
        int damage_,
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

    friend void swap(Enemy &lhs, Enemy &rhs) noexcept {
        using std::swap;
        swap(static_cast<Entity &>(lhs), static_cast<Entity &>(rhs));
        swap(lhs.damage, rhs.damage);
        swap(lhs.target, rhs.target);
        swap(lhs.isMoving, rhs.isMoving);
        swap(lhs.canDealDamage, rhs.canDealDamage);
        swap(lhs.state, rhs.state);
        swap(lhs.detectionRange, rhs.detectionRange);
        swap(lhs.attackRange, rhs.attackRange);
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

    Enemy& operator=(const Enemy& other) {
        if (this != &other) {
            const std::unique_ptr<Entity> clonedEntity = other.clone();
            auto* clonedEnemy = dynamic_cast<Enemy*>(clonedEntity.get());
            using std::swap;
            swap(*this, *clonedEnemy);
        }
        return *this;
    }

    Enemy (const Enemy& other);
    std::unique_ptr<Entity> clone() const override;
    ~Enemy() override;

    static int getActiveEnemyCount();
    int attackPlayer();
};

#endif