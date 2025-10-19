#pragma once

#include <iostream>
#include <string>
#include <Weapon.h>

class Player {
    std::string name;
    Weapon currWeapon;
    int health;
    float posX, posY;

public:
    Player(const std::string& n, const Weapon& w);
};