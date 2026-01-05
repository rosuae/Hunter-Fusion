#ifndef ENTITY_H
#define ENTITY_H

#include <SFML/Graphics.hpp>
#include <string>

class Map;
class Player;

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
    bool m_needsRemoval = false;

    static constexpr int PRIORITY_HAZARD = 100;
    static constexpr int PRIORITY_PICKUP = 50;
    static constexpr int PRIORITY_PORTAL = 10;
    static constexpr int PRIORITY_NONE = 0;

    sf::Texture* texture;
    sf::Sprite sprite;
    void setPosition(float x, float y);
    void updateSpritePosition();
    void checkDeath();

public:
    Entity(std::string n, float x, float y, float spd, float grav, sf::Texture& tex, int w, int h);
    virtual ~Entity() = default;

    Entity(const Entity &other) = default;

    friend void swap(Entity &lhs, Entity &rhs) noexcept {
        using std::swap;
        swap(lhs.name, rhs.name);
        swap(lhs.alive, rhs.alive);
        swap(lhs.max_health, rhs.max_health);
        swap(lhs.health, rhs.health);
        swap(lhs.hitbox, rhs.hitbox);
        swap(lhs.hitboxWidth, rhs.hitboxWidth);
        swap(lhs.hitboxHeight, rhs.hitboxHeight);
        swap(lhs.posX, rhs.posX);
        swap(lhs.posY, rhs.posY);
        swap(lhs.speed, rhs.speed);
        swap(lhs.gravity, rhs.gravity);
        swap(lhs.texture, rhs.texture);
        swap(lhs.sprite, rhs.sprite);
    }

    Entity & operator=(const Entity &other) {
        if (this == &other) {
            return *this;
        }
        const auto copy = other.clone();
        using std::swap;
        swap(*this, *copy);
        return *this;
    }

    virtual std::unique_ptr<Entity> clone() const = 0;

    virtual bool isObstacle() const { return false; }
    virtual bool isPet() const { return false; }
    virtual void draw(sf::RenderWindow& window) const;
    virtual void interactWithPlayer(Player&){}
    virtual void spawnAt(float x, float y);
    virtual void onDeath(Map&){}
    virtual void onCollision(Player&){}
    virtual int getCollisionPriority() const { return PRIORITY_NONE; }

    void updateHitbox();
    void behavior(float deltaTime, const Map& map);
    void tryHit(int damageAmount);

    bool isAlive() const;
    bool needsRemoval() const { return m_needsRemoval; }
    sf::FloatRect getBounds() const;
    sf::Vector2f getPos() const;
};

#endif // ENTITY_H