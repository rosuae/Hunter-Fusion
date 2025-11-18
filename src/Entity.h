#ifndef ENTITY_H
#define ENTITY_H

#include <SFML/Graphics.hpp>
#include <string>

class Map;

class Entity {

    bool alive;

    virtual sf::FloatRect doGetBounds() const = 0;
    virtual void doDraw(sf::RenderWindow& window) const = 0;
    virtual void doTakeDamage(int damageAmount) = 0;
    virtual void doUpdate(float deltaTime, const Map&) = 0;
    virtual void applyGravity(float deltaTime) = 0;
protected:
    std::string name;
    int health;
    float posX;
    float posY;
    float speed;
    float gravity;

    sf::Texture* texture;
    sf::Sprite sprite;
    void updateSpritePosition();
    void checkDeath();

public:
    Entity(std::string n, float x, float y, float spd, float grav, sf::Texture& tex);
    Entity (const Entity& other);
    // Entity& operator= (const Entity& other) {
    //     if (this == &other) {
    //         return *this;
    //     }
    //
    //     alive = other.alive;
    //     name = other.name;
    //     health = other.health;
    //     posX = other.posX;
    //     posY = other.posY;
    //     speed = other.speed;
    //     gravity = other.gravity;
    //     texture = other.texture;
    //     sprite = other.sprite;
    //     return *this;
    // }

    friend void swap(Entity &lhs, Entity &rhs) noexcept {
        using std::swap;
        swap(lhs.alive, rhs.alive);
        swap(lhs.name, rhs.name);
        swap(lhs.health, rhs.health);
        swap(lhs.posX, rhs.posX);
        swap(lhs.posY, rhs.posY);
        swap(lhs.speed, rhs.speed);
        swap(lhs.gravity, rhs.gravity);
        swap(lhs.texture, rhs.texture);
        swap(lhs.sprite, rhs.sprite);
    }

    Entity& operator= (const Entity& other) {
        if (this != &other) {
            auto copie = other.clone();
            using std::swap;
            swap(*this, *copie);
        }
        return *this;
    }

    virtual std::unique_ptr<Entity> clone() const = 0;
    virtual ~Entity();

    void draw(sf::RenderWindow& window) const;
    void update(float deltaTime, const Map& map);
    void takeDamage(int damageAmount);
    void setPosition(float x, float y);

    bool isAlive() const;

    sf::FloatRect getBounds() const;
    sf::Vector2f getPos() const;
};

#endif // ENTITY_H