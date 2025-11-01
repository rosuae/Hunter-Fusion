#include "Weapon.h"

[[nodiscard]]int Weapon::calculateReloadAmount() const {
    int needed = 30 - reloada;
    return std::min(needed, ammoamount);
}

[[nodiscard]]bool Weapon::hasAmmoToReload() const {
    return ammoamount > 0 && reloada < 30;
}

void Weapon::transferAmmo(int amount) {
    reloada += amount;
    ammoamount -= amount;
}

Weapon::Weapon(std::string n, std::string projName, int projDmg, const std::string& texturePath, int a):
    nume{std::move (n)},
    projectileName{std::move(projName)},
    reloada{30},
    ammoamount{a},
    projectileDmg{projDmg},
    firerate{0.1f}
    {
    if (!projectileTex.loadFromFile(texturePath))
        std::cout << "Eroare la incarcarea texturii Projectile";
    std::cout<<"Constructor weapon \n";
}

void Weapon::fire(sf::Vector2f playerPos, sf::Vector2f targetPos, std::list<sf::Sound>& sounds, const sf::SoundBuffer& buffer) {
    if (canFire()) {
        projectiles.emplace_back(projectileName, projectileDmg, projectileTex, playerPos, targetPos);
        reloada -= 1;

        sounds.emplace_back(buffer);
        sounds.back().play();

        std::cout << "Fire! \n";
    } else {
        std::cout << "press r to reload\n";
    }
}

void Weapon::updateProjectiles(float deltaTime) {
    for (auto& proj : projectiles) {
        if (proj.isActive()) {
            proj.projectileTravel(deltaTime);
        }
    }

    std::erase_if(projectiles, [](const Projectile& p) {
        return p.shouldBeRemoved(1920.f, 1080.f);
    });
}

void Weapon::reload(std::list<sf::Sound>& sounds, const sf::SoundBuffer& buffer) {
    if (hasAmmoToReload()) {
        int toReload = calculateReloadAmount();
        transferAmmo(toReload);

        sounds.emplace_back(buffer);
        sounds.back().play();

        std::cout << "\n Reloaded:  " << toReload << "\n";
        std::cout << "Ammo Amount: " << ammoamount << "\n";
    }
    else
        std::cout << "Out of ammo \n";
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

[[nodiscard]]bool Weapon::canFire() const {
    return reloada > 0;
}