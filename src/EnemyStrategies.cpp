#include "EnemyStrategies.h"
#include "Enemy.h"
#include "Utils.h"
#include <cmath>
#include <limits>

//
// Patrol Strategy
//
PatrolStrategy::PatrolStrategy(const float duration, const float direction)
    : patrolTimer(Utils::getRandom<float>(0.0f, duration)), patrolDuration(duration), patrolDirection(direction) {}

void PatrolStrategy::update(Enemy& enemy, const float deltaTime) {
    if (enemy.getDistanceToTarget() < enemy.getDetectionRange()) {
        enemy.setStrategy(std::make_unique<ChaseStrategy>());
        return;
    }

    patrolTimer += deltaTime;
    enemy.setMoving(true);

    if (patrolTimer >= patrolDuration) {
        patrolTimer = 0.0f;
        patrolDirection *= -1.0f;
    }

    const float currentX = enemy.getPos().x;
    const float speed = enemy.getSpeed();
    
    enemy.setX(currentX + speed * 0.5f * patrolDirection * deltaTime);

    if (patrolDirection > 0) enemy.setScale(1.f, 1.f);
    else enemy.setScale(-1.f, 1.f);
}

void PatrolStrategy::onWallCollision(Enemy&) {
    patrolDirection *= -1.0f;
    patrolTimer = 0.0f;
}

std::unique_ptr<EnemyStrategy> PatrolStrategy::clone() const {
    return std::make_unique<PatrolStrategy>(*this);
}

//
// Chase Strategy
//
ChaseStrategy::ChaseStrategy(const float aggroDuration) : aggroTimer(aggroDuration) {}

void ChaseStrategy::update(Enemy& enemy, const float deltaTime) {
    float distToPlayer = enemy.getDistanceToTarget();
    
    if (distToPlayer > enemy.getDetectionRange() * 1.5f && aggroTimer <= 0.0f) {
        enemy.setStrategy(std::make_unique<PatrolStrategy>());
        return;
    }

    if (enemy.isTouchingTarget()) {
        enemy.setStrategy(std::make_unique<AttackStrategy>());
        return;
    }

    if (aggroTimer > 0.0f) aggroTimer -= deltaTime;

    const sf::Vector2f targetPos = enemy.getTargetPos();
    const float currentX = enemy.getPos().x;
    const float diffX = targetPos.x - currentX;
    const float speed = enemy.getSpeed();

    if (std::abs(diffX) > 5.0f) {
        enemy.setMoving(true);

        if (diffX < 0) {
            enemy.setX(currentX - speed * deltaTime);
            enemy.setScale(-1.f, 1.f);
        } else {
            enemy.setX(currentX + speed * deltaTime);
            enemy.setScale(1.f, 1.f);
        }
    } else {
        enemy.setMoving(false);
    }
}

std::unique_ptr<EnemyStrategy> ChaseStrategy::clone() const {
    return std::make_unique<ChaseStrategy>(*this);
}

//
// Attack Strategy
//
AttackStrategy::AttackStrategy(const float cooldown)
    : currentAttackTimer(0.35f), attackCooldown(cooldown), canDealDamage(false) {}

void AttackStrategy::update(Enemy& enemy, const float deltaTime) {
    if (!enemy.isTouchingTarget()) {
        enemy.setStrategy(std::make_unique<ChaseStrategy>());
        return;
    }

    if (currentAttackTimer > 0.0f) {
        currentAttackTimer -= deltaTime;
        canDealDamage = false;
    } else {
        canDealDamage = true;
    }
    
    if (canDealDamage) {
        enemy.tryDealDamageToTarget(enemy.getDamage());
        resetAttack();
    }
}

void AttackStrategy::resetAttack() {
    canDealDamage = false;
    currentAttackTimer = attackCooldown;
}

std::unique_ptr<EnemyStrategy> AttackStrategy::clone() const {
    return std::make_unique<AttackStrategy>(*this);
}