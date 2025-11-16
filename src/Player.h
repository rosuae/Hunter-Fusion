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

    sf::FloatRect doGetBounds() const override;
    void applyGravity(float deltaTime) override;
    void doTakeDamage(int damageAmount) override;
    void updateSpriteDirection();
    float calculateWeaponOffsetX() const;

public:
    Player(const std::string& n, sf::Texture& tex);
    ~Player() override;

    void PlayerMovement(float deltaTime, std::list<sf::Sound>& sounds,
                       const sf::SoundBuffer& buffer, const Map& map);
    void updateAnimation(float deltaTime);
    void shootAnimation();
    void setFacing(bool isFacingRight);
    void setHit(bool ok);
    void alphaDamageEffect(int alpha);
    void resetDamageEffect();

    void draw(sf::RenderWindow& window) const;
    void drawDamageEffect(sf::RenderWindow& window) const;

    sf::Vector2f getWeaponTipPos() const;
    bool isHit() const;
    bool Jumping() const;
};

#endif