#include "Weapon.h"
#include "Map.h"
#include "Player.h"
#include "GameExceptions.h"

[[nodiscard]]int Weapon::calculateReloadAmount() const {
    int needed = 30 - reloada;
    return std::min(needed, ammoamount);
}

void Weapon::transferAmmo(int amount) {
    reloada += amount;
    ammoamount -= amount;
}

Weapon::Weapon(std::string n, std::string projName, int projDmg, sf::Texture& tex, const int a):
    nume{std::move (n)},
    projectileName{std::move(projName)},
    reloada{30},
    ammoamount{a},
    projectileDmg{projDmg},
    firerate{0.1f},
    projectileTex{&tex} {}

void Weapon::fire(Player& player, sf::Vector2f playerPos, sf::Vector2f targetPos, std::list<sf::Sound>& sounds, const sf::SoundBuffer& buffer) {
    bool shouldFaceRight = targetPos.x > playerPos.x;
    player.setFacing(shouldFaceRight);

    if (canFire(player)) {
        player.shootAnimation();
        projectiles.emplace_back(projectileName, projectileDmg, *projectileTex, playerPos, targetPos);
        reloada -= 1;

        sounds.emplace_back(buffer);
        sounds.back().play();
    }
}

void Weapon::updateProjectiles(float deltaTime, const Map& map) {
    for (auto& proj : projectiles) {
        if (proj.isActive()) {
            proj.projectileTravel(deltaTime, map);
        }
    }

    std::erase_if(projectiles, [](const Projectile& p) {
        return p.shouldBeRemoved();
    });
}

void Weapon::reload(std::list<sf::Sound>& sounds, const sf::SoundBuffer& buffer) {
    if (reloada >= 30) {
        throw InvalidActionException("Reload" , "Magazine Full");
    }

    if (ammoamount <= 0) {
        throw InvalidActionException("Reload", "Out of ammo");
    }

    int toReload = calculateReloadAmount();
    transferAmmo(toReload);

    sounds.emplace_back(buffer);
    sounds.back().play();
}

void Weapon::drawProjectiles(sf::RenderWindow& window) const {
    for (const auto& proj : projectiles) {
        proj.drawProjectile(window);
    }
}

Weapon::~Weapon() { std::cout << "S a apelat destructor Weapon \n";}

[[nodiscard]]std::vector<Projectile>& Weapon::getProjectiles() {
    return projectiles;
}

[[nodiscard]]int Weapon::getDmg() const{
    return projectileDmg;
}

[[nodiscard]]bool Weapon::canFire(const Player& player) const {
    return reloada > 0 && !player.Jumping();
}