#ifndef OOP_PROJECTILE_H
#define OOP_PROJECTILE_H

#include <iostream>
#include <cmath>

#include <SFML/Graphics.hpp>

class Map;

class Projectile {
    std::string nume;
    int dmg;
    float speed;
    bool active;
    sf::Vector2f position;
    sf::Vector2f direction;
    sf::Sprite sprite;

    void setupSprite(const sf::Texture& tex);
    void calculateDirection(sf::Vector2f playerPos, sf::Vector2f targetPos);
    void updatePosition(float deltaTime, const Map& map);

public:
    Projectile(std::string n, int d, const sf::Texture& tex, sf::Vector2f playerPos, sf::Vector2f targetPos);

    friend std::ostream& operator<< (std::ostream& out, const Projectile& p) {
        out << " Nume munitie: " << p.nume << " " << "DMG: "<< p.dmg << "\n";
        return out;
    }

    void drawProjectile(sf::RenderWindow& window) const;
    void projectileTravel (float deltaTime,  const Map& map);
    void deactivate();

    ~Projectile() = default;

    sf::FloatRect getBounds() const;
    [[nodiscard]]int getDamage() const;
    [[nodiscard]]bool isActive() const;
    // [[nodiscard]]bool isOutOfBounds(float maxX, float maxY) const;
    [[nodiscard]]bool shouldBeRemoved() const;
};


#endif
