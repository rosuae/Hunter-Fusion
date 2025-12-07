//
// Created by rosua on 12/6/2025.
//

#include "EnemyFactory.h"
#include "Enemy.h"
#include "ResourceManager.h"
#include "GameExceptions.h"

std::unique_ptr<Enemy> EnemyFactory::metroidPrototype = nullptr;

std::unique_ptr<Enemy> EnemyFactory::createEnemy(const std::string &type,
    const float x, const float y,
    ResourceManager &resManager,
    std::list<sf::Sound> &playingSounds,
    Entity *target)
{
    std::unique_ptr<Enemy> enemy = nullptr;

    if (type == "Metroid") {
        enemy.reset(new Enemy(
            type,
            20,
            x, y,
            resManager.getTexture("enemy.png"),
            playingSounds,
            resManager.getSound("enemydamage.wav"),
            resManager.getSound("enemydeath.wav")
        ));
    }
    if (enemy) {
        enemy->setPosition(x, y);
        enemy->setTarget(target);
    } else {
        throw ResourceException("Unknown enemy type: " + type);
    }

    return enemy;
}