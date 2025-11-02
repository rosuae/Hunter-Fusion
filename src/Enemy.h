#ifndef OOP_ENEMY_H
#define OOP_ENEMY_H

#include <list>

#include "Weapon.h"

class Enemy {
    std::string nume;
    int health;
    float posX, posY, gravity, speed;
    bool alive;
    Weapon* fists;
    sf::Texture& texture;
    sf::Sprite sprite;

    void moveTowardsPlayer(sf::Vector2f playerPos, float deltaTime);
    void applyGravity(float deltaTime);
    void updateSpritePosition();
    void checkDeath(std::list<sf::Sound>& sounds, const sf::SoundBuffer& buffer);

public:
    Enemy(std::string n, Weapon* f, float posx_, float posy_, sf::Texture& tex);

    void loadEnemy(sf::RenderWindow& window) const;
    void enemyMovement(sf::Vector2f playerpos, float deltaTime);
    void takeDamage(int damageAmount, std::list<sf::Sound>& sounds, const sf::SoundBuffer& buffer, const sf::SoundBuffer& buffer1);

    friend std::ostream& operator<< (std::ostream& out, const Enemy& e) {
        out << " Nume inamic: " << e.nume << " Pos X: " << e.posX << " Pos Y: " << e.posY << " Viata inamic: " << e.health << " Viteza imanic: " << e.speed << " " << *e.fists;
        return out;
    }

    Enemy& operator= (const Enemy& other) {
        if (this == &other) {
            return *this;
        }

        nume = other.nume;
        health = other.health;
        posX = other.posX;
        posY = other.posY;
        speed = other.speed;
        gravity = other.gravity;
        alive = other.alive;
        fists = other.fists;
        texture = other.texture;
        sprite = other.sprite;
        std::cout<<"S-a folosit supraincarcarea op= pentru clasa Enemy \n";
        return *this;
    }

    Enemy (const Enemy& other);
    ~Enemy();


    sf::FloatRect getBounds() const;
    [[nodiscard]]int getContactDamage() const;
    [[nodiscard]]bool isAlive() const;
};

#endif