#pragma once

#include <iostream>
#include <string>
#include <Projectile.h>

class Weapon {
    std::string nume;
    Projectile ammo;
    float firerate;
    int ammoamount, reloada;
public:
    Weapon(const std::string& n, const Projectile& p);

    void use();

private:
    void setAmmo(int ammoAmount);

    void decreaseAmmo();

    void reload();
};