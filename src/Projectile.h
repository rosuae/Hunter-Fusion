#ifndef OOP_PROJECTILE_H
#define OOP_PROJECTILE_H

#include <SFML/Graphics.hpp>

class Map;
class Entity;

class Projectile {
    std::string nume;
    int dmg;
    float speed;
    float distanceTraveled;
    float maxDistance;
    bool active;
    sf::Vector2f position;
    sf::Vector2f direction;
    sf::Sprite sprite;

    void setupSprite(const sf::Texture& tex);

public:
    Projectile(std::string n, int d, const sf::Texture& tex, sf::Vector2f playerPos, sf::Vector2f direction);

    friend std::ostream& operator<< (std::ostream& out, const Projectile& p) {
        out << " Nume munitie: " << p.nume << " " << "DMG: "<< p.dmg << "\n";
        return out;
    }

    std::unique_ptr<Projectile> clone() const {
        return std::make_unique<Projectile>(*this);
    }

    bool tryHit(Entity& target);
    void drawProjectile(sf::RenderWindow& window) const;
    void update (float deltaTime,  const Map& map);

    sf::FloatRect getBounds() const;
    [[nodiscard]]bool isActive() const;
    // [[nodiscard]]bool isOutOfBounds(float maxX, float maxY) const;
    [[nodiscard]]bool shouldBeRemoved() const;
};


#endif
