#pragma once
#include <string>
#include <Projectile.h>

class Weapon {
    std::string nume;
    Projectile ammo;
    float firerate;
    int ammoamount, reloada;
public:
    Weapon(const std::string& n, const Projectile& p):
    nume{n}, ammo{p}, firerate{0.1}, ammoamount{120}, reloada{30}{}

    void use() {
        ammo.travel();
        decreaseAmmo();
    }

private:
    void setAmmo(int ammoAmount) { ammoamount = ammoAmount; }
    void decreaseAmmo() { if (reloada > 0) reloada--; }
    void reload() {
        if (reloada > 0) {
            int needed = 30 - reloada;
            int toReload = std::min(needed, ammoamount);
            reloada += toReload;
            ammoamount -= toReload;
        }
    }

};