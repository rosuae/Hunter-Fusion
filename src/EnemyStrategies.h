#ifndef ENEMY_STRATEGIES_H
#define ENEMY_STRATEGIES_H

#include <memory>

class Enemy;

class EnemyStrategy {
public:
    virtual ~EnemyStrategy() = default;
    virtual void update(Enemy& enemy, float deltaTime) = 0;
    virtual std::unique_ptr<EnemyStrategy> clone() const = 0;
    virtual void onWallCollision(Enemy&) {}

    virtual bool isAttacking() const { return false; }
    virtual bool isChasing() const { return false; }
};

class PatrolStrategy : public EnemyStrategy {
    float patrolTimer;
    float patrolDuration;
    float patrolDirection;

public:
    explicit PatrolStrategy(float duration = 3.0f, float direction = 1.0f);
    void update(Enemy& enemy, float deltaTime) override;
    void onWallCollision(Enemy& enemy) override;
    std::unique_ptr<EnemyStrategy> clone() const override;
};

class ChaseStrategy : public EnemyStrategy {
    float aggroTimer;

public:
    explicit ChaseStrategy(float aggroDuration = 0.0f);
    void update(Enemy& enemy, float deltaTime) override;
    std::unique_ptr<EnemyStrategy> clone() const override;
    bool isChasing() const override { return true; }
};

class AttackStrategy : public EnemyStrategy {
    float currentAttackTimer;
    float attackCooldown;
    bool canDealDamage;

public:
    explicit AttackStrategy(float cooldown = 0.8f);
    void update(Enemy& enemy, float deltaTime) override;
    std::unique_ptr<EnemyStrategy> clone() const override;
    
    // bool canAttack() const { return canDealDamage; }
    void resetAttack();
    bool isAttacking() const override { return true; }
};

#endif // ENEMY_STRATEGIES_H