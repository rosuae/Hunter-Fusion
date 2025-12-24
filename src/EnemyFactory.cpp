//
// Created by rosua on 12/6/2025.
//

#include "EnemyFactory.h"
#include "Enemy.h"
#include "ResourceManager.h"
#include "GameExceptions.h"

std::map<std::string, std::unique_ptr<Enemy>> EnemyFactory::registry;

void EnemyFactory::loadPrototype(const std::string& type, ResourceManager& res, std::list<sf::Sound>& sounds, Entity* target) {
    if (registry.contains(type)) return;

    if (type == "Metroid") {
        registry[type].reset(new Enemy(
            "Metroid", 20, 150, 0, 0,
            res.getTexture("enemy.png"),
            res.getTexture("enemyAgro.png"),
            sounds,
            res.getSound("enemydamage.wav"),
            res.getSound("enemydeath.wav"),
            target
        ));
    }
    else {
        throw ResourceException("Unknown enemy type in loader: " + type);
    }
}

std::unique_ptr<Enemy> EnemyFactory::createEnemy(const std::string &type,
    const float x, const float y,
    ResourceManager &resManager,
    std::list<sf::Sound> &playingSounds,
    Entity *target)
{
    loadPrototype(type, resManager, playingSounds, target);
    std::unique_ptr<Enemy> newEnemy = std::make_unique<Enemy>(*registry[type]);

    newEnemy->setPosition(x, y);
    return newEnemy;
}