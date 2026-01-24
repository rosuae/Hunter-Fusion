#include "Enemy.h"
#include "Player.h"
#include "Pickup.h"
#include "GameExceptions.h"
#include "Game.h"
#include "Map.h"
#include "Utils.h"
#include <SFML/Audio.hpp>
#include <cmath>

Enemy::Enemy(const std::string& n, const int damage_, const int bountyScore_, const float posx_, const float posy_, sf::Texture& tex, const sf::Texture& alertTex,
             std::list<sf::Sound>& activeSounds_,
             const sf::SoundBuffer& hitSound_,
             const sf::SoundBuffer& deathSound_,
             Entity* target_)
    : Entity(n, posx_, posy_, 200.f, 1000.f, tex, 100, 180),
    damage{damage_},
    bountyScore{bountyScore_},
    target{target_},
    isMoving{false},
    strategy{std::make_unique<PatrolStrategy>()},
    detectionRange{400.f},
    currentFrame{0},
    animationTimer{0.f},
    frameDuration{0.1f},
    animationFrameCount{8},
    alertTexture{&alertTex},
    exclamationSprite{*alertTexture},
    alertAnimTimer{0.f},
    alertActive{false},
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

    alertFrameSize.x = static_cast<int>(alertTexture->getSize().x);
    alertFrameSize.y = static_cast<int>(alertTexture->getSize().y) / 2;

    exclamationSprite.setOrigin({
        static_cast<float>(alertFrameSize.x) / 2.f,
        static_cast<float>(alertFrameSize.y) / 2.f}
    );

    exclamationSprite.setTextureRect(sf::IntRect({0, 0}, {alertFrameSize.x, alertFrameSize.y}));
    exclamationSprite.setScale({0.f, 0.f});
}

Enemy::Enemy(const Enemy &other)
    : Entity(other),
      damage(other.damage),
      bountyScore(other.bountyScore),
      target(other.target),
      isMoving(other.isMoving),
      detectionRange(other.detectionRange),
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
    
    if (other.strategy) {
        strategy = other.strategy->clone();
    } else {
        strategy = std::make_unique<PatrolStrategy>();
    }
    
    activeEnemyCount++;
}

void swap(Enemy &lhs, Enemy &rhs) noexcept {
    using std::swap;
    swap(static_cast<Entity &>(lhs), static_cast<Entity &>(rhs));
    swap(lhs.damage, rhs.damage);
    swap(lhs.bountyScore, rhs.bountyScore);
    swap(lhs.target, rhs.target);
    swap(lhs.isMoving, rhs.isMoving);
    swap(lhs.strategy, rhs.strategy);
    swap(lhs.detectionRange, rhs.detectionRange);
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

void Enemy::updateAI(const float deltaTime) {
    if (!target || !target->isAlive()) {
        if (!strategy || strategy->isChasing() || strategy->isAttacking()) {
            setStrategy(std::make_unique<PatrolStrategy>());
        }
    }
    
    if (strategy) {
        strategy->update(*this, deltaTime);
    }
}

void Enemy::setStrategy(std::unique_ptr<EnemyStrategy> newStrategy) {
    strategy = std::move(newStrategy);
}

float Enemy::getDistanceToTarget() const {
    if (!target) return std::numeric_limits<float>::max();
    return std::hypot(target->getPos().x - posX, target->getPos().y - posY);
}

bool Enemy::isTouchingTarget() const {
    if (!target) return false;
    return this->getBounds().findIntersection(target->getBounds()).has_value();
}

sf::Vector2f Enemy::getTargetPos() const {
    if (target) return target->getPos();
    return {posX, posY};
}

void Enemy::tryDealDamageToTarget(const int dmgAmount) const {
    if (target && isTouchingTarget()) {
        if (auto* playerPtr = dynamic_cast<Player*>(target)) {
            playerPtr->tryHit(dmgAmount);
        }
    }
}


void Enemy::grantReward(Player& player) const {
    player.processKill(this->bountyScore);
}

void Entity::spawnAt(const float x, const float y) {
    setPosition(x, y);
    updateHitbox();
}

void Enemy::onDeath(Map& map) {
    if (target && dynamic_cast<Player*>(target)) {
        auto* p = dynamic_cast<Player*>(target);
        grantReward(*p);
    }

    map.onEnemyKilled();

    if (Utils::getRandom<int>(1, 100) <= 25) {
        try {
            constexpr float groundOffsetY = 40;
            auto drop = PickupFactory::create(
                PickupType::Ammo,
                posX,
                posY - groundOffsetY,
                map.getResourceManager(),
                map.getSoundList()
            );

            if (drop) {
                map.spawnEntityAt(std::move(drop));
            }
        } catch (const ResourceException& e) {
            std::cout << "[Drop Error] " << e.what() << std::endl;
        }
    }
}

// void Enemy::onCollision(Player &player) {
//     if (this->canDealDamage && this->state == EnemyState::Attacking) {
//         player.tryHit(this->damage);
//         this->resetAttackTimer();
//     }
// }
// i can use this later on if implementing a knockback

sf::FloatRect Enemy::doGetBounds() const{
    return hitbox;
}

void Enemy::takeDamage(const int damageAmount) {
    setStrategy(std::make_unique<ChaseStrategy>(5.0f));

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
    const float startX = posX;

    isMoving = false;
    updateAI(deltaTime);
    updatePhysics(deltaTime, map);
    if (std::abs(posX - startX) < 0.05f) {
        isMoving = false;
    }
    updateAnimation(deltaTime);
    sprite.setPosition(sf::Vector2f(posX, posY));
}

void Enemy::updatePhysics(const float deltaTime, const Map &map) {
    updateHitbox();
    if (map.isWall(hitbox, true)) {
        if (strategy) {
            strategy->onWallCollision(*this);
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
    const bool attacking = strategy && strategy->isAttacking();
    const bool chasing = strategy && strategy->isChasing();

    if (attacking) {
        currentFrame = animationFrameCount - 1;
        animationTimer = 0.0f;
        const int rectLeft = currentFrame * frameSize.x;
        sprite.setTextureRect(sf::IntRect({rectLeft, 0}, {frameSize.x - 2, frameSize.y}));
    }
    else if (!isMoving) {
        const int rectLeft = currentFrame * frameSize.x;
        sprite.setTextureRect(sf::IntRect({rectLeft, 0}, {frameSize.x - 2, frameSize.y}));
    }
    else {
        animationTimer += deltaTime;
        if (animationTimer >= frameDuration) {
            animationTimer -= frameDuration;
            currentFrame++;
            if (currentFrame >= animationFrameCount) {
                currentFrame = 0;
            }
        }

        const int rectLeft = currentFrame * frameSize.x;
        sprite.setTextureRect(sf::IntRect({rectLeft, 0}, {frameSize.x - 2, frameSize.y}));
    }

    const float headTopY = posY - sprite.getOrigin().y;
    const float halfAlertHeight = static_cast<float>(alertFrameSize.y) / 2.f;
    constexpr float padding = 5.f;

    float yCorrection = 0.f;
    if (attacking) {
        yCorrection = -5.f;
    }

    exclamationSprite.setPosition({posX, headTopY - padding - halfAlertHeight + yCorrection});

    bool shouldShowAlert = (chasing || attacking);

    if (shouldShowAlert) {
        if (!alertActive) {
            alertActive = true;
            alertAnimTimer = 0.0f;
        }

        alertAnimTimer += deltaTime;

        constexpr float duration = 0.2f;
        float scale = alertAnimTimer / duration;

        if (scale > 1.0f) scale = 1.0f;

        exclamationSprite.setScale({scale, scale});

        if (chasing) {
            exclamationSprite.setTextureRect(sf::IntRect({0, 0},
                                            {alertFrameSize.x, alertFrameSize.y}));
        }
        else if (attacking) {
            exclamationSprite.setTextureRect(sf::IntRect({0, alertFrameSize.y},
                                            {alertFrameSize.x, alertFrameSize.y}));
        }
    }
    else {
        alertActive = false;
        alertAnimTimer = 0.0f;
        exclamationSprite.setScale({0.f, 0.f});
    }
}

void Enemy::draw(sf::RenderWindow& window) const {
    if (!alive) return;
    window.draw(sprite);

    if (strategy && (strategy->isChasing() || strategy->isAttacking())) {
        window.draw(exclamationSprite);
    }
}

void Enemy::applyGravity(const float deltaTime) {
    posY += gravity * deltaTime;
}

int Enemy::activeEnemyCount = 0;

int Enemy::getActiveEnemyCount() {
    return activeEnemyCount;
}