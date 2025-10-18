#pragma once
#include <string>

class Projectile {
    std::string nume;
    int dmg;

public:
    Projectile(const std::string& n, int d): nume{n}, dmg{d}{}
    void travel();
};