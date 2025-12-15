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

    const int magCapacity = 30;
    int reloada;
    int ammoAmount;
    int max_ammo;
    int current_ammo;

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

    Weapon(const Weapon &other) :
    state(other.state),
    nume(other.nume),
    projectileName(other.projectileName),
    reloada(other.reloada),
    ammoAmount(other.ammoAmount),
    max_ammo(other.max_ammo),
    current_ammo(other.current_ammo),
    projectileDmg(other.projectileDmg),
    fireRate(other.fireRate),
    fireTimer(other.fireTimer),
    reloadDuration(other.reloadDuration),
    reloadTimer(other.reloadTimer),
    projectiles(other.projectiles),
    projectileTex(other.projectileTex),
    activeSounds(other.activeSounds),
    shootSound(other.shootSound),
    reloadSound(other.reloadSound) {
        std::cout << "Weapon CC";
    }

    Weapon(Weapon &&other) noexcept :
    state(other.state),
    nume(std::move(other.nume)),
    projectileName(std::move(other.projectileName)),
    reloada(other.reloada),
    ammoAmount(other.ammoAmount),
    max_ammo(other.max_ammo),
    current_ammo(other.current_ammo),
    projectileDmg(other.projectileDmg),
    fireRate(other.fireRate),
    fireTimer(other.fireTimer),
    reloadDuration(other.reloadDuration),
    reloadTimer(other.reloadTimer),
    projectiles(std::move(other.projectiles)),
    projectileTex(other.projectileTex),
    activeSounds(other.activeSounds),
    shootSound(other.shootSound),
    reloadSound(other.reloadSound) {
    }

    Weapon& operator=(const Weapon& other) {
        if (this != &other){
            const auto copie = other.clone();
            using std::swap;
            swap(*this, *copie);
            std::cout << "Copy and swap\n";
        }
        return *this;
    }

    [[nodiscard]]std::unique_ptr<Weapon> clone() const;

    friend std::ostream& operator<< (std::ostream& out, const Weapon& w) {
        out << " Numar total munitie: " << w.ammoAmount << " Firerate: " << w.fireRate << " Nume arma: " << w.nume << " Munitie per reload: " << w.reloada;
        return out;
    }

    bool tryFire(const Player& player, sf::Vector2f direction);
    bool tryReload();

    void resetAmmo();
    void clearProjectiles();
    void update(float deltaTime, const Map& map);
    void draw(sf::RenderWindow& window) const;
    void handleCollisions(const std::vector<std::unique_ptr<Entity>>& targets);

    ~Weapon();

    [[nodiscard]]int getAmmoInClip() const { return reloada; }
    [[nodiscard]]int getTotalAmmo() const { return ammoAmount; }
};

#endif