#include "Enemy.h"
#include "Map.h"
#include <SFML/Audio.hpp>
#include <iostream>

Enemy::Enemy(const std::string& n, const int damage_, float posx_, float posy_, sf::Texture& tex,
    std::list<sf::Sound>& activeSounds_,
    const sf::SoundBuffer& hitSound_,
    const sf::SoundBuffer& deathSound_)
    : Entity(n, posx_, posy_, 300.f, 1000.f, tex, 120, 120),
    damage{damage_},
    target{nullptr},
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
}

Enemy::Enemy (const Enemy& other)
: Entity(other),
damage{other.damage},
target{other.target},
activeSounds{other.activeSounds},
hitSound{other.hitSound},
deathSound{other.deathSound}
{
    std::cout<<"Constructor de copiere\n";
}

std::unique_ptr<Entity> Enemy::clone() const{
    return std::make_unique<Enemy>(*this);
}

Enemy::~Enemy() {std::cout << "S a apelat destructor Enemy \n";}

void Enemy::moveTowardsPlayer(sf::Vector2f playerPos, float deltaTime) {
    if (posX >= playerPos.x) {
        posX -= speed * deltaTime;
    }
    if (posX <= playerPos.x) {
        posX += speed * deltaTime;
    }
}

sf::FloatRect Enemy::doGetBounds() const{
    float left = posX - static_cast<float>(hitboxWidth) / 2.0f;
    float top = posY - static_cast<float>(hitboxHeight);
    return sf::FloatRect({left, top},
        {static_cast<float>(hitboxWidth), static_cast<float>(hitboxHeight)});
}

void Enemy::doTakeDamage(int damageAmount) {

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

    if (!target || !target->isAlive()) {
        float lastY = posY;
        applyGravity(deltaTime);

        float topLeftX = posX - static_cast<float>(hitboxWidth) / 2.f;
        float topLeftY = posY - static_cast<float>(hitboxHeight);
        sf::FloatRect enemyBoundsY(
                sf::Vector2f(topLeftX, topLeftY),
                sf::Vector2f(static_cast<float>(hitboxWidth), static_cast<float>(hitboxHeight))
                );

        if (map.isWall(enemyBoundsY, true)) { posY = lastY; }

        return;
    }

    float lastX = posX;

    moveTowardsPlayer(sf::Vector2f(target->getPos()), deltaTime);

    float topLeftX = posX - static_cast<float>(hitboxWidth) / 2.f;
    float topLeftY = posY - static_cast<float>(hitboxHeight);

    sf::FloatRect enemyBoundsX(
        sf::Vector2f(topLeftX, topLeftY),
        sf::Vector2f(static_cast<float>(hitboxWidth), static_cast<float>(hitboxHeight))
        );

    if (map.isWall(enemyBoundsX, true)) {
        posX = lastX;
    }

    float lastY = posY;

    applyGravity(deltaTime);

    topLeftX = posX - static_cast<float>(hitboxWidth) / 2.f;
    topLeftY = posY - static_cast<float>(hitboxHeight);

    sf::FloatRect enemyBoundsY(
        sf::Vector2f(topLeftX, topLeftY),
        sf::Vector2f(static_cast<float>(hitboxWidth), static_cast<float>(hitboxHeight))
        );

    if (map.isWall(enemyBoundsY, true)) {
        posY = lastY;
    }
}

void Enemy::applyGravity(float deltaTime) {
    posY += gravity * deltaTime;
}

void Enemy::setTarget(Entity *playerTarget) {
    this->target = playerTarget;
}

int Enemy::getContactDamage() const {
    return damage;
}