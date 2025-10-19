#pragma once

#include <iostream>
#include <string>

class Projectile {
    std::string nume;
    int dmg, speed;

public:
    Projectile(const std::string& n, int d);

    static void travel();
};