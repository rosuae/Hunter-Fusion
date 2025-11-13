#ifndef PLAYER_H
#define PLAYER_H

#include "Entity.h"
#include <SFML/Audio.hpp>
#include <list>

class Map;

class Player : public Entity {
    float velocity;
    float maxJump;

    sf::Vector2i frameSize;
    int hitboxWidth;
    int hitboxHeight;

    float animationTimer;
    float frameDuration;
    float shootingTimer;
    static constexpr float shootingDuration = 0.3f;

    bool isRunning;
    bool isJumping;
    bool facingRight;
    bool isHit_;

    int currentFrame;
    int animationRow;
    int animationStartIndex;
    int animationFrameCount;

    sf::RectangleShape damageOverlay;

    void updateSpriteDirection();
    void applyGravity(float deltaTime) override;
    void checkDeath() override;
    float calculateWeaponOffsetX() const;

public:
    Player(const std::string& n, sf::Texture& tex);
    ~Player() override;

    void PlayerMovement(float deltaTime, std::list<sf::Sound>& sounds,
                       const sf::SoundBuffer& buffer, const Map& map);
    void updateAnimation(float deltaTime);
    void shootAnimation();
    void setFacing(bool isFacingRight);
    void takeDamage(int damageAmount) override;
    void setHit(bool ok);
    void alphaDamageEffect(int alpha);
    void resetDamageEffect();

    void draw(sf::RenderWindow& window) const override;
    void drawDamageEffect(sf::RenderWindow& window) const;

    sf::Vector2f getWeaponTipPos() const;
    sf::FloatRect getBounds() const override;
    bool isHit() const;
    bool Jumping() const;
};

#endif