#pragma once

#include <iostream>
#include <string>
#include <Weapon.h>

class Enemy {
    std::string nume;
    Weapon fists;
    float posX, posY;
    int health, speed;

public:
    Enemy(const std::string& n, const Weapon& f, float posx_, float posy_);

    Enemy(const Enemy& other);

    Enemy& operator=(const Enemy& other) {
        nume = other.nume;
        fists = other.fists;
        posX = other.posX;
        posY = other.posY;
        health = other.health;
        speed = other.speed;
        return *this;
    }

    ~Enemy() = default;

protected:
    void attack();
};