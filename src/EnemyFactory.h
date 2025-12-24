//
// Created by rosua on 12/6/2025.
//

#ifndef OOP_ENEMYFACTORY_H
#define OOP_ENEMYFACTORY_H

#include <memory>
#include <string>
#include <list>
#include <map>
#include <SFML/Audio.hpp>

class Enemy;
class ResourceManager;
class Entity;

class EnemyFactory {
    static std::map<std::string, std::unique_ptr<Enemy>> registry;
    static void loadPrototype(const std::string& type, ResourceManager& res, std::list<sf::Sound>& sounds, Entity* target);
public:
    static std::unique_ptr<Enemy> createEnemy(
        const std::string& type,
        float x, float y,
        ResourceManager& resManager,
        std::list<sf::Sound>& playingSounds,
        Entity* target
        );
};

#endif //OOP_ENEMYFACTORY_H