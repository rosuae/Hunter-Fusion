#ifndef OOP_ENEMY_H
#define OOP_ENEMY_H

#include <list>
#include "Weapon.h"
#include "Entity.h"

class Map;

class Enemy : public Entity{
    Entity* target;
    std::shared_ptr<Weapon> fists;

    std::list<sf::Sound>& activeSounds;
    const sf::SoundBuffer& hitSound;
    const sf::SoundBuffer& deathSound;

    sf::FloatRect doGetBounds() const override;
    void doDraw(sf::RenderWindow& window) const override;
    void doTakeDamage(int damageAmount) override;
    void doUpdate(float deltaTime, const Map &) override;
    void applyGravity(float deltaTime) override;
    void moveTowardsPlayer(sf::Vector2f playerPos, float deltaTime);

public:
    Enemy(const std::string& n,
        std::shared_ptr<Weapon> f,
        float posx_, float posy_,
        sf::Texture& tex,
        std::list<sf::Sound>& activeSounds_,
        const sf::SoundBuffer& hitSound_,
        const sf::SoundBuffer& deathSound_);

    void setTarget (Entity* playerTarget);

    friend std::ostream& operator<< (std::ostream& out, const Enemy& e) {
        out << " Nume inamic: " << e.name << " Pos X: " << e.posX << " Pos Y: " << e.posY << " Viata inamic: " << e.health << " Viteza imanic: " << e.speed << " " << *e.fists;
        return out;
    }

    Enemy& operator= (const Enemy& other) {
        if (this == &other) {
            return *this;
        }

        Entity::operator=(other);
        target = other.target;
        fists = other.fists;
        std::cout<<"S-a folosit supraincarcarea op= pentru clasa Enemy \n";
        return *this;
    }

    Enemy (const Enemy& other);
    ~Enemy() override;

    [[nodiscard]]int getContactDamage() const;
};

#endif