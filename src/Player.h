#pragma once

#include <string>
#include <Weapon.h>

class Player {
    std::string name;
    Weapon currWeapon;
    int health;
    float posX, posY;
public:
    Player(const std::string& n, const Weapon& w):
    name{n}, currWeapon{w}, health{100}, posX{0}, posY{0}{}
    void attack() {
        currWeapon.use();
    }

    void isPlaying();
};