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

    friend std::ostream& operator<< (std::ostream& out, const Weapon& w) {
        out << " Numar total munitie: " << w.ammoamount << " Firerate: " << w.firerate << " Nume arma: " << w.nume << " Munitie per reload: " << w.reloada;
        return out;
    }

    void fire(const Player& player, sf::Vector2f direction);
    void updateProjectiles(float deltaTime, const Map& map);
    void reload();
    void update(float deltaTime);
    void drawProjectiles(sf::RenderWindow& window) const;

    ~Weapon();

    [[nodiscard]]std::vector<Projectile>& getProjectiles();
    [[nodiscard]]int getDmg() const;
    [[nodiscard]]bool canFire(const Player& player) const;
    [[nodiscard]]int getAmmoInClip() const { return reloada; }
    [[nodiscard]]int getTotalAmmo() const { return ammoamount; }
};

#endif