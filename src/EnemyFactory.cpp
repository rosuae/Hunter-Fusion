//
// Created by rosua on 12/6/2025.
//

#include "EnemyFactory.h"
#include "Enemy.h"
#include "ResourceManager.h"
#include "GameExceptions.h"

std::unique_ptr<Enemy> EnemyFactory::metroidPrototype = nullptr;

std::unique_ptr<Enemy> EnemyFactory::createEnemy(const std::string &type,
    const int damage,
    float x, float y,
    ResourceManager &resManager,
    std::list<sf::Sound> &playingSounds,
    Entity *target)
{
    std::unique_ptr<Entity> clonedEntity = nullptr;

    if (type == "Metroid") {
        if (!metroidPrototype) {
            metroidPrototype = std::unique_ptr<Enemy>(new Enemy(
                type,
                damage,
                0, 0,
                resManager.getTexture("enemy.png"),
                playingSounds,
                resManager.getSound("enemydamage.wav"),
                resManager.getSound("enemydeath.wav")
            ));
        }
        clonedEntity = metroidPrototype->clone();
    }
    auto enemy = std::unique_ptr<Enemy>(dynamic_cast<Enemy*>(clonedEntity.release()));

    if (enemy) {
        enemy->setPosition(x, y);
        enemy->setTarget(target);
    } else {
        throw ResourceException("Unknown enemy type: " + type);
    }

    return enemy;
}