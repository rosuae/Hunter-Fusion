#ifndef OOP_ENEMY_H
#define OOP_ENEMY_H

#include <list>
#include "Weapon.h"
#include "Entity.h"
#include "EnemyFactory.h"

class Map;

class Enemy : public Entity{
    int damage;
    static int activeEnemyCount;
    Entity* target;

    std::list<sf::Sound>* activeSounds;
    const sf::SoundBuffer* hitSound;
    const sf::SoundBuffer* deathSound;

    sf::FloatRect doGetBounds() const override;
    void doTakeDamage(int damageAmount) override;
    void doBehavior(float deltaTime, const Map& map) override;
    void applyGravity(float deltaTime) override;
    void moveTowardsPlayer(sf::Vector2f playerPos, float deltaTime);

    friend class EnemyFactory;
protected:

    Enemy(const std::string& n,
        int damage_,
        float posx_, float posy_,
        sf::Texture& tex,
        std::list<sf::Sound>& activeSounds_,
        const sf::SoundBuffer& hitSound_,
        const sf::SoundBuffer& deathSound_);

public:
    void setTarget (Entity* playerTarget);

    friend std::ostream& operator<< (std::ostream& out, const Enemy& e) {
        out << " Nume inamic: " << e.name << " Pos X: " << e.posX << " Pos Y: " << e.posY << " Viata inamic: " << e.health << " Viteza imanic: " << e.speed;
        return out;
    }

    friend void swap(Enemy &lhs, Enemy &rhs) noexcept {
        using std::swap;
        swap(static_cast<Entity &>(lhs), static_cast<Entity &>(rhs));
        swap(lhs.damage, rhs.damage);
        swap(lhs.target, rhs.target);
        swap(lhs.activeSounds, rhs.activeSounds);
        swap(lhs.hitSound, rhs.hitSound);
        swap(lhs.deathSound, rhs.deathSound);
    }

    Enemy& operator=(const Enemy& other) {
        if (this != &other){
            auto copie = other.clone();
            using std::swap;
            swap(*this, *copie);
            std::cout << "COPY AND SWAP!!!!!!!!!!!!!!!!!!!!!!!!!!\n";
        }
        return *this;
    }

    Enemy (const Enemy& other);
    std::unique_ptr<Entity> clone() const override;
    ~Enemy() override;

    int getContactDamage() const;
    static int getActiveEnemyCount();
};

#endif