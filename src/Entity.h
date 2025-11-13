#ifndef ENTITY_H
#define ENTITY_H

#include <SFML/Graphics.hpp>
#include <string>

class Map;

class Entity {
protected:
    std::string name;
    int health;
    float posX;
    float posY;
    float speed;
    float gravity;
    bool alive;

    sf::Texture& texture;
    sf::Sprite sprite;

public:
    Entity(std::string n, float x, float y, float spd, float grav, sf::Texture& tex);
    Entity (const Entity& other);
    virtual ~Entity();

    virtual void draw(sf::RenderWindow& window) const = 0;
    virtual sf::FloatRect getBounds() const = 0;
    virtual void takeDamage(int damageAmount) = 0;

    virtual void applyGravity(float deltaTime);
    virtual void updateSpritePosition();
    virtual void checkDeath();
    virtual bool isAlive() const;

    sf::Vector2f getPos() const;

    void setPosition(float x, float y);
};

#endif // ENTITY_H