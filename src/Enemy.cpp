#include "Enemy.h"
#include "Map.h"
#include <iostream>

Enemy::Enemy(const std::string& n, std::shared_ptr<Weapon> f, float posx_, float posy_, sf::Texture& tex)
    : Entity(n, posx_, posy_, 300.f, 1000.f, tex),
    target{nullptr},
    fists{std::move(f)}
{
    sprite.setPosition(sf::Vector2f(posX, posY));
    sprite.setOrigin(sf::Vector2f(static_cast<float>(texture.getSize().x) / 2.f,
                                    static_cast<float>(texture.getSize().y))
                                    );
    sprite.setScale(sf::Vector2f(1.f, 1.f));
}

Enemy::Enemy (const Enemy& other)
: Entity(other),
target{other.target},
fists{other.fists}
{
    std::cout<<"Constructor de copiere\n";
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
    return sprite.getGlobalBounds();
}

void Enemy::doDraw(sf::RenderWindow& window) const{
    window.draw(sprite);
}

void Enemy::doTakeDamage(int damageamount) {
    health -= damageamount;
}

void Enemy::doUpdate(float deltaTime, const Map& map) {

    sf::FloatRect localBounds = sprite.getLocalBounds();
    float spriteWidth = localBounds.size.x;
    float spriteHeigth = localBounds.size.y;

    if (!target || !target->isAlive()) {
        float lastY = posY;
        applyGravity(deltaTime);

        float topLeftX = posX - spriteWidth / 2.f;
        float topLeftY = posY - spriteHeigth;
        sf::FloatRect enemyBoundsY(
                sf::Vector2f(topLeftX, topLeftY),
                sf::Vector2f(spriteWidth, spriteHeigth)
                );

        if (map.isWall(enemyBoundsY)) { posY = lastY; }

        return;
    }

    float lastX = posX;

    moveTowardsPlayer(sf::Vector2f(target->getPos()), deltaTime);

    float topLeftX = posX - spriteWidth / 2.f;
    float topLeftY = posY - spriteHeigth;

    sf::FloatRect enemyBoundsX(
        sf::Vector2f(topLeftX, topLeftY),
        sf::Vector2f(spriteWidth, spriteHeigth)
        );

    if (map.isWall(enemyBoundsX)) {
        posX = lastX;
    }

    float lastY = posY;

    applyGravity(deltaTime);

    topLeftX = posX - spriteWidth / 2.f;
    topLeftY = posY - spriteHeigth;

    sf::FloatRect enemyBoundsY(
        sf::Vector2f(topLeftX, topLeftY),
        sf::Vector2f(spriteWidth, spriteHeigth)
        );

    if (map.isWall(enemyBoundsY)) {
        posY = lastY;
    }
}

void Enemy::applyGravity(float deltaTime) {
    posY += gravity * deltaTime;
}

void Enemy::setTarget(Entity *playerTarget) {
    this->target = playerTarget;
}

void Enemy::takeDamage(int damageAmount, std::list<sf::Sound>& sounds, const sf::SoundBuffer& hitBuff, const sf::SoundBuffer& deathBuff) {

    bool wasAlive = this->isAlive();

    Entity::takeDamage(damageAmount);

    if (isAlive()) {
        sounds.emplace_back(hitBuff);
        sounds.back().play();
    }
    else if (wasAlive) {
        sounds.emplace_back(deathBuff);
        sounds.back().play();
    }
}

int Enemy::getContactDamage() const {
    return fists->getDmg();
}