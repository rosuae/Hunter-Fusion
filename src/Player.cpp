#include <Player.h>

Player::Player(const std::string& n, const Weapon& w):
    name{n}, currWeapon{w}, health{100}, posX{0}, posY{0}{}
