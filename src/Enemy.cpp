#include "Enemy.h"
#include "Map.h"
#include <iostream>

void Enemy::moveTowardsPlayer(sf::Vector2f playerPos, float deltaTime) {
    if (posX >= playerPos.x) {
        posX -= speed * deltaTime;
    }
    if (posX <= playerPos.x) {
        posX += speed * deltaTime;
    }
}

void Enemy::takeDamage(int damageAmount) {
    health -= damageAmount;
    checkDeath();
}

Enemy::Enemy(std::string n, Weapon* f, float posx_, float posy_, sf::Texture& tex)
    : Entity(n, posx_, posy_, 300.f, 1000.f, tex),
    fists{f} {
    sprite.setPosition(sf::Vector2f(posX, posY));
    sprite.setOrigin(sf::Vector2f(static_cast<float>(texture.getSize().x) / 2.f,
                                    static_cast<float>(texture.getSize().y))
                                    );
    sprite.setScale(sf::Vector2f(1.f, 1.f));
}

void Enemy::draw(sf::RenderWindow& window) const{
    window.draw(sprite);
}

void Enemy::enemyMovement(sf::Vector2f playerpos, float deltaTime, const Map& map) {

    sf::FloatRect localBounds = sprite.getLocalBounds();
    float spriteWidth = localBounds.size.x;
    float spriteHeigth = localBounds.size.y;

    float lastX = posX;

    moveTowardsPlayer(playerpos, deltaTime);

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

    updateSpritePosition();
}

void Enemy::takeDamage(int damageAmount, std::list<sf::Sound>& sounds, const sf::SoundBuffer& hitBuff, const sf::SoundBuffer& deathBuff) {

    bool wasAlive = this->alive;

    this->takeDamage(damageAmount);

    if (isAlive()) {
        sounds.emplace_back(hitBuff);
        sounds.back().play();
    }
    else if (wasAlive) {
        sounds.emplace_back(deathBuff);
        sounds.back().play();
    }
}

Enemy::Enemy (const Enemy& other)
: Entity(other),
fists{other.fists}
{
    std::cout<<"Constructor de copiere\n";
}

Enemy::~Enemy() {std::cout << "S a apelat destructor Enemy \n";}

sf::FloatRect Enemy::getBounds() const{
    return sprite.getGlobalBounds();
}

int Enemy::getContactDamage() const {
    return fists->getDmg();
}