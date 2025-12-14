#ifndef ENTITY_H
#define ENTITY_H

#include <SFML/Graphics.hpp>
#include <string>

class Map;

class Entity {
    virtual sf::FloatRect doGetBounds() const = 0;
    virtual void takeDamage(int damageAmount) = 0;
    virtual void doBehavior(float deltaTime, const Map& map) = 0;
    virtual void applyGravity(float deltaTime) = 0;

protected:
    std::string name;
    bool alive;
    int max_health;
    int health;
    sf::FloatRect hitbox;
    int hitboxWidth;
    int hitboxHeight;
    float posX;
    float posY;
    float speed;
    float gravity;

    sf::Texture* texture;
    sf::Sprite sprite;
    void setPosition(float x, float y);
    void updateSpritePosition();
    void checkDeath();

public:
    Entity(std::string n, float x, float y, float spd, float grav, sf::Texture& tex, int w, int h);
    Entity (const Entity& other);

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

    virtual bool isObstacle() const { return false; }
    virtual std::unique_ptr<Entity> clone() const = 0;
    virtual void draw(sf::RenderWindow& window) const;
    virtual ~Entity();

    void updateHitbox();
    void behavior(float deltaTime, const Map& map);
    void tryHit(int damageAmount);

    bool isAlive() const;

    sf::FloatRect getBounds() const;
    sf::Vector2f getPos() const;
};

#endif // ENTITY_H