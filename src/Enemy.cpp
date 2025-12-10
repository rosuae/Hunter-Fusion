#include "Enemy.h"
#include "Map.h"
#include <SFML/Audio.hpp>
#include "Player.h"

Enemy::Enemy(const std::string& n, const int damage_, float posx_, float posy_, sf::Texture& tex,
             std::list<sf::Sound>& activeSounds_,
             const sf::SoundBuffer& hitSound_,
             const sf::SoundBuffer& deathSound_,
             Entity* target_)
    : Entity(n, posx_, posy_, 200.f, 1000.f, tex, 120, 120),
    damage{damage_},
    target{target_},
    // fists{std::move(f)},
    activeSounds{&activeSounds_},
    hitSound{&hitSound_},
    deathSound{&deathSound_}
{
    sprite.setPosition(sf::Vector2f(posX, posY));
    sprite.setOrigin(sf::Vector2f(static_cast<float>(texture->getSize().x) / 2.f,
                                    static_cast<float>(texture->getSize().y) - 10.f)
                                    );
    sprite.setScale(sf::Vector2f(1.f, 1.f));
    updateHitbox();
    activeEnemyCount++;
}

Enemy::Enemy (const Enemy& other)
: Entity(other),
damage{other.damage},
target{other.target},
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

void Enemy::updateAI(float deltaTime) {
    if (target && target->isAlive()) {
        if (posX > target->getPos().x) {
            posX -= speed * deltaTime;
            sprite.setScale({-1.f, 1.f});
        } else {
            posX += speed * deltaTime;
            sprite.setScale({1.f, 1.f});
        }
    }
}

sf::FloatRect Enemy::doGetBounds() const{
    return hitbox;
}

void Enemy::takeDamage(int damageAmount) {
    bool wasAlive = this->isAlive();
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

void Enemy::doBehavior(float deltaTime, const Map& map) {
    updateAI(deltaTime);
    updatePhysics(deltaTime, map);
    sprite.setPosition(sf::Vector2f(posX, posY));
}

int Enemy::attackPlayer() const {
    if (!target || !target->isAlive()) return 0;
    if (!isAlive()) return 0;

    if (hitbox.findIntersection(target->getBounds()).has_value())
        return damage;

    return 0;
}

void Enemy::updatePhysics(float deltaTime, const Map &map) {
    updateHitbox();
    if (map.isWall(hitbox, true)) {
        const float direction = sprite.getScale().x > 0.f ? 1.f : -1.f;
        posX -= speed * direction * deltaTime;
        updateHitbox();
    }

    float lastY = posY;
    applyGravity(deltaTime);

    if (posY != lastY) {
        updateHitbox();
        if (map.isWall(hitbox, true)) {
            posY = lastY;
            updateHitbox();
        }
    }
}

void Enemy::applyGravity(float deltaTime) {
    posY += gravity * deltaTime;
}

int Enemy::activeEnemyCount = 0;

int Enemy::getActiveEnemyCount() {
    return activeEnemyCount;
}