#include "Weapon.h"
#include "Map.h"
#include "Player.h"

[[nodiscard]]int Weapon::calculateReloadAmount() const {
    const int needed = magCapacity - reloada;
    return std::min(needed, ammoAmount);
}

void Weapon::transferAmmo(const int amount) {
    reloada += amount;
    ammoAmount -= amount;
}

Weapon::Weapon(std::string n, std::string projName, const int projDmg, sf::Texture& tex, const int a, std::list<sf::Sound>& activeSounds_, const sf::SoundBuffer& shootSound_, const sf::SoundBuffer& reloadSound_):
    state{WeaponState::Ready},
    nume{std::move (n)},
    projectileName{std::move(projName)},
    reloada{30},
    ammoAmount{a},
    max_ammo{ammoAmount},
    projectileDmg{projDmg},
    fireRate{0.2f},
    fireTimer{0.f},
    reloadDuration{1.f},
    reloadTimer{0.f},
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

bool Weapon::tryFire(const Player& player, const sf::Vector2f direction) {
    if (state == WeaponState::Reloading) return false;
    if (state == WeaponState::Cooldown) return false;

    if (!player.isAlive()) return false;
    if (reloada <= 0) {
        tryReload();
        return false;
    }
    sf::Vector2f spawnPoint = player.getWeaponTipPos();
    sf::Vector2f calculatedTarget = spawnPoint + direction * 1000.f;

    projectiles.emplace_back(projectileName, projectileDmg, *projectileTex, spawnPoint, calculatedTarget);

    reloada -= 1;

    activeSounds.emplace_back(shootSound);
    activeSounds.back().play();

    state = WeaponState::Cooldown;
    fireTimer = fireRate;

    return true;
}

bool Weapon::tryReload() {
    if (state == WeaponState::Reloading) return false;
    if (reloada >= magCapacity) return false;
    if (ammoAmount <= 0) return false;

    state = WeaponState::Reloading;
    reloadTimer = reloadDuration;

    activeSounds.emplace_back(reloadSound);
    activeSounds.back().play();
    return true;
}

void Weapon::finishReload() {
    const int toReload = calculateReloadAmount();
    transferAmmo(toReload);
}

void Weapon::resetAmmo() {
    reloada = magCapacity;
    ammoAmount = max_ammo;

    state = WeaponState::Ready;
    fireTimer = 0.0f;
    reloadTimer = 0.0f;
}

bool Weapon::stockAmmo(const int amount) {
    if (ammoAmount >= max_ammo) {
        return false;
    }
    ammoAmount += amount;
    if (ammoAmount > max_ammo) {
        ammoAmount = max_ammo;
    }
    return true;
}

void Weapon::clearProjectiles() {
    projectiles.clear();
}

void Weapon::update(const float deltaTime, const Map& map) {
    if (state == WeaponState::Cooldown) {
        fireTimer -= deltaTime;
        if (fireTimer <= 0.0f) {
            state = WeaponState::Ready;
        }
    }

    else if (state == WeaponState::Reloading) {
        reloadTimer -= deltaTime;
        if (reloadTimer <= 0.0f) {
            finishReload();
            state = WeaponState::Ready;
        }
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


Weapon::~Weapon() { std::cout << "Weapon destructor \n";}