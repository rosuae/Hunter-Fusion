#include "Enemy.h"

void Enemy::moveTowardsPlayer(sf::Vector2f playerPos, float deltaTime) {
    if (posX >= playerPos.x) {
        posX -= speed * deltaTime;
    }
    if (posX <= playerPos.x) {
        posX += speed * deltaTime;
    }
}

void Enemy::applyGravity(float deltaTime) {
    posY += gravity * deltaTime;
    if (posY >= 1000.f)
        posY = 1000.f;
}

void Enemy::updateSpritePosition() {
    sprite.setPosition(sf::Vector2f(posX, posY));
}

void Enemy::checkDeath(std::list<sf::Sound>& sounds, const sf::SoundBuffer& buffer) {
    if (health <= 0) {
        health = 0;

        sounds.emplace_back(buffer);
        sounds.back().play();

        alive = false;
        std::cout << "inamic invins \n";
    }
}

Enemy::Enemy(std::string n, Weapon* f, float posx_, float posy_, const std::string& texturePath):
    nume{std::move(n)},
    health{100},
    posX{posx_},
    posY(posy_),
    gravity{3000.f},
    speed{300.f},
    alive{true},
    fists{f},
    texture{texturePath},
    sprite{texture} {
    if (!texture.loadFromFile(texturePath))
        std::cout << "Eroare la incarcarea texturii Enemy";

    sprite.setPosition(sf::Vector2f(posX, posY));
    sprite.setOrigin(sf::Vector2f(static_cast<float>(texture.getSize().x) / 2.f, static_cast<float>(texture.getSize().y)));
    sprite.setScale(sf::Vector2f(.7f, .7f));
    std::cout<<"Constructor Enemy \n";
}

void Enemy::loadEnemy(sf::RenderWindow& window) const{
    window.draw(sprite);
}

void Enemy::enemyMovement(sf::Vector2f playerpos, float deltaTime) {
    moveTowardsPlayer(playerpos, deltaTime);
    applyGravity(deltaTime);
    updateSpritePosition();
}

void Enemy::takeDamage(int damageAmount, std::list<sf::Sound>& sounds, const sf::SoundBuffer& buffer, const sf::SoundBuffer& buffer1) {
    health -= damageAmount;

    sounds.emplace_back(buffer);
    sounds.back().play();

    checkDeath(sounds, buffer1);
    if (alive) {
        std::cout << "a primit " << damageAmount << " dmg, mai are " << health << "\n";
    }
}

Enemy::Enemy (const Enemy& other):
nume{other.nume},
health{other.health},
posX{other.posX},
posY{other.posY},
gravity{other.gravity},
speed{other.speed},
alive{other.alive},
fists{other.fists},
texture{other.texture},
sprite{other.sprite}
{
    sprite.setTexture(texture);
    std::cout<<"Constructor de copiere\n";
}

Enemy::~Enemy() {std::cout << "S a apelat destructor Enemy \n";}

sf::FloatRect Enemy::getBounds() const{
    return sprite.getGlobalBounds();
}

int Enemy::getContactDamage() const {
    return fists->getDmg();
}

bool Enemy::isAlive() const{
    return alive;
}