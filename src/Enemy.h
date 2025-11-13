#ifndef OOP_ENEMY_H
#define OOP_ENEMY_H

#include <list>
#include "Weapon.h"
#include "Entity.h"

class Map;

class Enemy : public Entity{
    Weapon* fists;

    void moveTowardsPlayer(sf::Vector2f playerPos, float deltaTime);
public:
    Enemy(std::string n, Weapon* f, float posx_, float posy_, sf::Texture& tex);

    void draw(sf::RenderWindow& window) const override;
    void enemyMovement(sf::Vector2f playerpos, float deltaTime, const Map& map);
    void takeDamage(int damageAmount) override;
    void takeDamage(int damageAmount, std::list<sf::Sound>& sounds, const sf::SoundBuffer& buffer, const sf::SoundBuffer& buffer1);

    friend std::ostream& operator<< (std::ostream& out, const Enemy& e) {
        out << " Nume inamic: " << e.name << " Pos X: " << e.posX << " Pos Y: " << e.posY << " Viata inamic: " << e.health << " Viteza imanic: " << e.speed << " " << *e.fists;
        return out;
    }

    Enemy& operator= (const Enemy& other) {
        if (this == &other) {
            return *this;
        }

        name = other.name;
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
    ~Enemy() override;


    sf::FloatRect getBounds() const override;
    [[nodiscard]]int getContactDamage() const;
};

#endif