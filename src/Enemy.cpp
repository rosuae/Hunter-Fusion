#include <Enemy.h>

Enemy::Enemy(const std::string& n, const Weapon& f, float posx_, float posy_):
    nume{n},
    fists{f},
    posX{posx_},
    posY{posy_},
    health{100},
    speed{100}{}

Enemy::Enemy(const Enemy& other):
    nume{other.nume},
    fists{other.fists},
    posX{other.posX},
    posY{other.posY},
    health{other.health},
    speed{other.speed}{}

void Enemy::attack(){
    fists.use();
}