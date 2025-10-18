#include <iostream>
#include <vector>

class Projectile {
    std::string nume;
    int dmg;

public:
    Projectile(const std::string& n, int d): nume{n}, dmg{d}{}
    void travel();
};

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

    ~Enemy() {}

    void attack() {
        fists.use();
    }
};

class Map {
    std::string MapNume;
    int sizeX, sizeY;
    Player MyPlayer;
    std::vector<Enemy> enemies;

public:
    Map(const std::string& n, int sizex_, int sizey_, const Player& p):
    MapNume{n}, sizeX{sizex_}, sizeY{sizey_}, MyPlayer{p}{}

    void addEnemy(const Enemy& enemy) {
        enemies.push_back(enemy);
    }
};