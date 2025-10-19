#include <Weapon.h>

Weapon::Weapon(const std::string& n, const Projectile& p):
    nume{n}, ammo{p}, firerate{0.1}, ammoamount{120}, reloada{30}{}

void Weapon::use() {
    ammo.travel();
    decreaseAmmo();
}

void Weapon::setAmmo(int ammoAmount) { ammoamount = ammoAmount; }

void Weapon::decreaseAmmo() { if (reloada > 0) reloada--; }

void Weapon::reload() {
    if (reloada > 0) {
        int needed = 30 - reloada;
        int toReload = std::min(needed, ammoamount);
        reloada += toReload;
        ammoamount -= toReload;
    }
}