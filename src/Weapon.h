#ifndef OOP_WEAPON_H
#define OOP_WEAPON_H

#include "Projectile.h"

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>

#include <string>
#include <vector>
#include <list>

class Map;
class Player;
class Entity;

class Weapon {
    std::string nume;
    std::string projectileName;
    int reloada;
    int ammoamount;
    int projectileDmg;
    float firerate;
    float fireTimer;

    std::vector<Projectile> projectiles;
    sf::Texture* projectileTex;

    std::list<sf::Sound>& activeSounds;
    sf::Sound shootSound, reloadSound;

    [[nodiscard]]int calculateReloadAmount() const;
    void transferAmmo(int amount);

public:
    Weapon(std::string n, std::string projName, int projDmg, sf::Texture& tex, int a, std::list<sf::Sound>& activeSounds_, const sf::SoundBuffer& shootSound_, const sf::SoundBuffer& reloadSound_);

    Weapon(const Weapon &other)
        : nume(other.nume),
          projectileName(other.projectileName),
          reloada(other.reloada),
          ammoamount(other.ammoamount),
          projectileDmg(other.projectileDmg),
          firerate(other.firerate),
          fireTimer(other.fireTimer),
          projectiles(other.projectiles),
          projectileTex(other.projectileTex),
          activeSounds(other.activeSounds),
          shootSound(other.shootSound),
          reloadSound(other.reloadSound) {
    }

    Weapon(Weapon &&other) noexcept
        : nume(std::move(other.nume)),
          projectileName(std::move(other.projectileName)),
          reloada(other.reloada),
          ammoamount(other.ammoamount),
          projectileDmg(other.projectileDmg),
          firerate(other.firerate),
          fireTimer(other.fireTimer),
          projectiles(std::move(other.projectiles)),
          projectileTex(other.projectileTex),
          activeSounds(other.activeSounds),
          shootSound(std::move(other.shootSound)),
          reloadSound(std::move(other.reloadSound)) {
    }

    Weapon& operator=(const Weapon& other) {
        if (this != &other){
            auto copie = other.clone();
            using std::swap;
            swap(*this, *copie);
            std::cout << "Copy and swap\n";
        }
        return *this;
    }

    [[nodiscard]]std::unique_ptr<Weapon> clone() const;

    friend std::ostream& operator<< (std::ostream& out, const Weapon& w) {
        out << " Numar total munitie: " << w.ammoamount << " Firerate: " << w.firerate << " Nume arma: " << w.nume << " Munitie per reload: " << w.reloada;
        return out;
    }

    bool fire(const Player& player, sf::Vector2f direction);
    bool reload();

    void update(float deltaTime, const Map& map);
    void draw(sf::RenderWindow& window) const;
    void handleCollisions(const std::vector<std::unique_ptr<Entity>>& targets);

    ~Weapon();

    [[nodiscard]]std::vector<Projectile>& getProjectiles();
    [[nodiscard]]bool canFire(const Player& player) const;
    [[nodiscard]]int getAmmoInClip() const { return reloada; }
    [[nodiscard]]int getTotalAmmo() const { return ammoamount; }
};

#endif