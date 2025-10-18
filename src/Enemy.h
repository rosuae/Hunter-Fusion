#pragma once

#include <string>
#include <Weapon.h>

class Enemy {
    std::string nume;
    Weapon fists;
    float posX, posY;
    int health, speed;

public:
    Enemy(const std::string& n, const Weapon& f, float posx_, float posy_):
    nume{n}, fists{f}, posX{posx_}, posY{posy_}, health{100}, speed{100}{}

    Enemy(const Enemy& other):
    nume{other.nume}, fists{other.fists}, posX{other.posX}, posY{other.posY}, health{other.health}, speed{other.speed}{}

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

    void attack() {
        fists.use();
    }
};