#include "Weapon.h"
#include "Map.h"
#include "Player.h"
#include "GameExceptions.h"

[[nodiscard]]int Weapon::calculateReloadAmount() const {
    const int needed = 30 - reloada;
    return std::min(needed, ammoamount);
}

void Weapon::transferAmmo(const int amount) {
    reloada += amount;
    ammoamount -= amount;
}

Weapon::Weapon(std::string n, std::string projName, int projDmg, sf::Texture& tex, const int a, std::list<sf::Sound>& activeSounds_, const sf::SoundBuffer& shootSound_, const sf::SoundBuffer& reloadSound_):
    nume{std::move (n)},
    projectileName{std::move(projName)},
    reloada{30},
    ammoamount{a},
    projectileDmg{projDmg},
    firerate{0.125f},
    fireTimer{0.f},
    projectileTex{&tex},
    activeSounds{activeSounds_},
    shootSound{shootSound_},
    reloadSound{reloadSound_}
{
    shootSound.setVolume(30); //until volume settings feature
    reloadSound.setVolume(30);
}

std::unique_ptr<Weapon> Weapon::clone() const {
    return std::make_unique<Weapon>(*this);
}

bool Weapon::fire(const Player& player, const sf::Vector2f direction) {
    if (canFire(player)) {
        sf::Vector2f spawnPoint = player.getWeaponTipPos();
        sf::Vector2f calculatedTarget = spawnPoint + direction * 1000.f;

        projectiles.emplace_back(projectileName, projectileDmg, *projectileTex, spawnPoint, calculatedTarget);

        reloada -= 1;
        fireTimer = firerate;

        activeSounds.emplace_back(shootSound);
        activeSounds.back().play();

        return true;
    }
    return false;
}

bool Weapon::reload() {
    if (reloada >= 30) {
        throw InvalidActionException("Reload" , "Magazine Full");
    }

    if (ammoamount <= 0) {
        throw InvalidActionException("Reload", "Out of ammo");
    }

    int toReload = calculateReloadAmount();
    transferAmmo(toReload);

    activeSounds.emplace_back(reloadSound);
    activeSounds.back().play();

    return true;
}

void Weapon::clearProjectiles() {
    projectiles.clear();
}

void Weapon::update(float deltaTime, const Map& map) {
    if (fireTimer > 0.0f) {
        fireTimer -= deltaTime;
    }

    for (auto& proj : projectiles) {
        if (proj.isActive()) {
            proj.update(deltaTime, map);
        }
    }

    std::erase_if(projectiles, [](const Projectile& p) {
        return p.shouldBeRemoved();
    });
}

void Weapon::draw(sf::RenderWindow& window) const {
    for (const auto& proj : projectiles) {
        proj.drawProjectile(window);
    }
}

void Weapon::handleCollisions(const std::vector<std::unique_ptr<Entity> > &targets) {
    for (auto& proj : projectiles) {
        if (!proj.isActive()) continue;

        for (const auto& target : targets)
            if (proj.tryHit(*target))
                break;
    }
}


Weapon::~Weapon() { std::cout << "S a apelat destructor Weapon \n";}

[[nodiscard]]bool Weapon::canFire(const Player& player) const {
    const bool weaponReady = reloada > 0 && fireTimer <= 0.0f;
    const bool playerReady = player.canAttack() && player.isAlive();
    return weaponReady && playerReady;
}