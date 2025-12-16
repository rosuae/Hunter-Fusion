#include "Enemy.h"
#include "Map.h"
#include <SFML/Audio.hpp>
#include "Player.h"
#include <cmath>

Enemy::Enemy(const std::string& n, const int damage_, const float posx_, const float posy_, sf::Texture& tex,
             std::list<sf::Sound>& activeSounds_,
             const sf::SoundBuffer& hitSound_,
             const sf::SoundBuffer& deathSound_,
             Entity* target_)
    : Entity(n, posx_, posy_, 200.f, 1000.f, tex, 120, 180),
    damage{damage_},
    target{target_},
    state{EnemyState::Patrolling},
    detectionRange{600.f},
    attackRange{60.f},
    attackCooldown{0.5f},
    currentAttackTimer{0.f},
    aggroTimer{0.f},
    patrolTimer{0.f},
    patrolDuration{4.f},
    patrolDirection{1.f},
    currentFrame{0},
    animationTimer{0.f},
    frameDuration{0.1f},
    animationFrameCount{8},
    activeSounds{&activeSounds_},
    hitSound{&hitSound_},
    deathSound{&deathSound_}
{
    frameSize.x = static_cast<int>(tex.getSize().x) / animationFrameCount;
    frameSize.y = static_cast<int>(tex.getSize().y);

    sprite.setPosition(sf::Vector2f(posX, posY));

    sprite.setOrigin(sf::Vector2f(
        static_cast<float>(frameSize.x) / 2.f,
        static_cast<float>(frameSize.y) - 10.f
    ));

    sprite.setScale(sf::Vector2f(1.f, 1.f));
    sprite.setTextureRect(sf::IntRect({0, 0}, {frameSize.x, frameSize.y}));

    updateHitbox();
    activeEnemyCount++;
}

Enemy::Enemy (const Enemy& other)
: Entity(other),
damage{other.damage},
target{other.target},
state{EnemyState::Patrolling},
detectionRange{other.detectionRange},
attackRange{other.attackRange},
attackCooldown{other.attackCooldown},
currentAttackTimer{other.currentAttackTimer},
aggroTimer{other.aggroTimer},
patrolTimer{other.patrolTimer},
patrolDuration{other.patrolDuration},
patrolDirection{other.patrolDirection},
currentFrame{other.currentFrame},
animationTimer{other.animationTimer},
frameDuration{other.frameDuration},
animationFrameCount{other.animationFrameCount},
activeSounds{other.activeSounds},
hitSound{other.hitSound},
deathSound{other.deathSound}
{
    activeEnemyCount++;
}

std::unique_ptr<Entity> Enemy::clone() const{
    return std::make_unique<Enemy>(*this);
}

Enemy::~Enemy() {activeEnemyCount--;}

void Enemy::updateAI(const float deltaTime) {
    if (!target || !target->isAlive()) {
        state = EnemyState::Patrolling;
    }

    if (aggroTimer > 0.0f) {
        aggroTimer -= deltaTime;
    }

    float distToPlayer = 99999.f;
    if (target) {
        const float dx = target->getPos().x - posX;
        const float dy = target->getPos().y - posY;
        distToPlayer = std::sqrt(dx*dx + dy*dy);
    }

    switch (state) {
        case EnemyState::Patrolling:
            if (distToPlayer < detectionRange) {
                state = EnemyState::Chasing;
            }
            updatePatrol(deltaTime);
            break;

        case EnemyState::Chasing:
            if (distToPlayer > detectionRange * 1.5f && aggroTimer <= 0.0f) {
                state = EnemyState::Patrolling;
            }
            else if (distToPlayer < attackRange) {
                state = EnemyState::Attacking;
            }
            else {
                updateChase(deltaTime);
            }
            break;

        case EnemyState::Attacking:
            if (distToPlayer > attackRange) {
                state = EnemyState::Chasing;
            }
            updateAttack(deltaTime);
            break;
    }
}

void Enemy::updatePatrol(const float deltaTime) {
    patrolTimer += deltaTime;

    if (patrolTimer >= patrolDuration) {
        patrolTimer = 0.0f;
        patrolDirection *= -1.0f;
    }

    posX += speed * 0.5f * patrolDirection * deltaTime;

    if (patrolDirection > 0) sprite.setScale({1.f, 1.f});
    else sprite.setScale({-1.f, 1.f});
}

void Enemy::updateChase(const float deltaTime) {
    if (posX > target->getPos().x) {
        posX -= speed * deltaTime;
        sprite.setScale({-1.f, 1.f});
    } else {
        posX += speed * deltaTime;
        sprite.setScale({1.f, 1.f});
    }
}

void Enemy::updateAttack(const float deltaTime) {
    if (currentAttackTimer > 0.0f) {
        currentAttackTimer -= deltaTime;
    }
}

int Enemy::attackPlayer() {
    if (!isAlive()) return 0;
    if (!target || !target->isAlive()) return 0;

    if (currentAttackTimer > 0.0f) return 0;

    const float dx = target->getPos().x - posX;
    const float dy = target->getPos().y - posY;

    const float distSq = dx*dx + dy*dy;

    if (const float attackRangeSq = attackRange * attackRange; distSq <= attackRangeSq) {
        currentAttackTimer = attackCooldown;
        return damage;
    }

    return 0;
}

sf::FloatRect Enemy::doGetBounds() const{
    return hitbox;
}

void Enemy::takeDamage(const int damageAmount) {
    state = EnemyState::Chasing;
    aggroTimer = 5.0f;
    const bool wasAlive = this->isAlive();
    health -= damageAmount;
    checkDeath();

    if (isAlive()) {
        activeSounds->emplace_back(*hitSound);
        activeSounds->back().play();
    }
    else if (wasAlive) {
        activeSounds->emplace_back(*deathSound);
        activeSounds->back().play();
    }
}

void Enemy::doBehavior(const float deltaTime, const Map& map) {
    updateAI(deltaTime);
    updatePhysics(deltaTime, map);
    updateAnimation(deltaTime);
    sprite.setPosition(sf::Vector2f(posX, posY));
}

void Enemy::updatePhysics(const float deltaTime, const Map &map) {
    updateHitbox();
    if (map.isWall(hitbox, true)) {
        if (state == EnemyState::Patrolling) {
            patrolDirection *= -1.f;
            patrolTimer = 0.0f;
        }
        const float dir = sprite.getScale().x > 0.f ? 1.f : -1.f;
        posX -= speed * dir * deltaTime;
        updateHitbox();
    }
    const float lastY = posY;
    applyGravity(deltaTime);

    if (posY != lastY) {
        updateHitbox();
        if (map.isWall(hitbox, true)) {
            posY = lastY;
            updateHitbox();
        }
    }
}

void Enemy::updateAnimation(const float deltaTime) {
    if (state == EnemyState::Attacking) {
        currentFrame = animationFrameCount - 1;
    }
    else {
        animationTimer += deltaTime;
        if (animationTimer >= frameDuration) {
            animationTimer -= frameDuration;
            currentFrame = (currentFrame + 1) % animationFrameCount;
        }
    }

    const int rectLeft = currentFrame * frameSize.x;
    constexpr int safetyGap = 2;
    sprite.setTextureRect(sf::IntRect({rectLeft, 0}, {frameSize.x - safetyGap, frameSize.y}));
}

void Enemy::applyGravity(const float deltaTime) {
    posY += gravity * deltaTime;
}

int Enemy::activeEnemyCount = 0;

int Enemy::getActiveEnemyCount() {
    return activeEnemyCount;
}