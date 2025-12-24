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

enum class WeaponState {
    Ready,
    Cooldown,
    Reloading,
    Empty
};

class Weapon {
    WeaponState state;

    std::string nume;
    std::string projectileName;

    const int magCapacity;
    int reloada;
    int ammoAmount;
    int max_ammo;

    int projectileDmg;
    float fireRate;
    float fireTimer;

    float reloadDuration;
    float reloadTimer;

    std::vector<Projectile> projectiles;
    sf::Texture* projectileTex;

    std::list<sf::Sound>& activeSounds;
    sf::Sound shootSound, reloadSound;

    void finishReload();
    [[nodiscard]]int calculateReloadAmount() const;
    void transferAmmo(int amount);

public:
    Weapon(std::string n, std::string projName, int projDmg, sf::Texture& tex, int a, std::list<sf::Sound>& activeSounds_, const sf::SoundBuffer& shootSound_, const sf::SoundBuffer& reloadSound_);
    [[nodiscard]]std::unique_ptr<Weapon> clone() const;

    friend std::ostream& operator<< (std::ostream& out, const Weapon& w) {
        out << " Numar total munitie: " << w.ammoAmount << " Firerate: " << w.fireRate << " Nume arma: " << w.nume << " Munitie per reload: " << w.reloada;
        return out;
    }

    bool tryFire(const Player& player, sf::Vector2f direction);
    bool tryReload();

    void resetAmmo();
    bool stockAmmo(int amount);
    void clearProjectiles();
    void update(float deltaTime, const Map& map);
    void draw(sf::RenderWindow& window) const;
    void handleCollisions(const std::vector<std::unique_ptr<Entity>>& targets);

    [[nodiscard]]int getAmmoInClip() const { return reloada; }
    [[nodiscard]]int getTotalAmmo() const { return ammoAmount; }
};

#endif