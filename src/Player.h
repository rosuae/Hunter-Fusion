#ifndef OOP_PLAYER_H
#define OOP_PLAYER_H

#include <string>
#include <list>
#include <iostream>

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>

class Map;

class Player {
    std::string name;
    int health;
    float posX, posY, speed, gravity, velocity, maxJump;
    bool isalive = true, isHit_ = false, facingRight = true;

    sf::Texture& texture;
    sf::Sprite sprite;
    sf::RectangleShape damageOverlay;

    sf::Vector2i frameSize;
    const float shootingDuration = 0.3f;
    float hitboxWidth;
    float hitboxHeight;
    float animationTimer;
    float frameDuration;
    float shootingTimer;

    bool isRunning;
    bool isJumping;

    int currentFrame;
    int animationRow;
    int animationStartIndex;
    int animationFrameCount;

    void updateSpriteDirection();
    void applyGravity(float deltaTime);
    void checkDeath(sf::RenderWindow& window);

    [[nodiscard]]float calculateWeaponOffsetX() const;

public:
    Player(std::string n, sf::Texture& tex);

    friend std::ostream& operator<< (std::ostream& out, const Player& p) {
        out << " Nume player: " << p.name << " Viata: " << p.health<< " Pos X: " << p.posX << " Pos Y: " << p.posY << "Player speed: " << p.speed;
        return out;
    }

    void PlayerMovement(float deltaTime, std::list<sf::Sound>& sounds, const sf::SoundBuffer& buffer, const Map& map);
    void updateAnimation(float deltaTime);
    void shootAnimation();
    void takeDamage (int damageAmount, sf::RenderWindow& window);
    void setHit (bool ok);
    void alphaDamageEffect(int alpha);
    void resetDamageEffect();
    void draw(sf::RenderWindow& window) const;
    void drawDamageEffect(sf::RenderWindow& window) const;

    ~Player();

    [[nodiscard]]sf::Vector2f getPos() const;
    [[nodiscard]]sf::Vector2f getWeaponTipPos() const;
    [[nodiscard]]sf::FloatRect getBounds () const;
    [[nodiscard]]bool isHit () const;
    [[nodiscard]]bool Jumping() const;
    [[nodiscard]]bool isAlive() const;

};

#endif
