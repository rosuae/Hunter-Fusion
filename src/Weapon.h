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
    std::vector<Projectile> projectiles;
    sf::Texture* projectileTex;

    [[nodiscard]]int calculateReloadAmount() const;

    void transferAmmo(int amount);

public:
    Weapon(std::string n, std::string projName, int projDmg, sf::Texture& tex, int a);

    friend std::ostream& operator<< (std::ostream& out, const Weapon& w) {
        out << " Numar total munitie: " << w.ammoamount << " Firerate: " << w.firerate << " Nume arma: " << w.nume << " Munitie per reload: " << w.reloada;
        return out;
    }

    void fire(Player& player, sf::Vector2f playerPos, sf::Vector2f targetPos, std::list<sf::Sound>& sounds, const sf::SoundBuffer& buffer);
    void updateProjectiles(float deltaTime, const Map& map);
    void reload(std::list<sf::Sound>& sounds, const sf::SoundBuffer& buffer);
    void drawProjectiles(sf::RenderWindow& window) const;

    ~Weapon();

    [[nodiscard]]std::vector<Projectile>& getProjectiles();
    [[nodiscard]]int getDmg() const;
    [[nodiscard]]bool canFire(const Player& player) const;
};

#endif