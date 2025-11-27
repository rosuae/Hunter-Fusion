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
    bool facingUp;
    bool isHit_;

    int currentFrame;
    int animationRow;
    int animationStartIndex;
    int animationFrameCount;

    sf::RectangleShape damageOverlay;
    std::list<sf::Sound>& activeSounds;
    sf::Sound jumpSound;

    sf::FloatRect doGetBounds() const override;
    void doDraw(sf::RenderWindow &window) const override;
    void doTakeDamage(int damageAmount) override;
    void doBehavior(float deltaTime, const Map&) override;
    void applyGravity(float deltaTime) override;
    void updateSpriteDirection();

public:
    Player(const std::string& n, sf::Texture& tex, float posx_, float posy_, std::list<sf::Sound>& activeSounds_, const sf::SoundBuffer& jumpSound_);
    std::unique_ptr<Entity> clone() const override;
    ~Player() override;

    void updateAnimation(float deltaTime);
    void shootAnimation();
    void setFacing(bool isFacingRight);
    void setFacingUp(bool isFacingUp);
    void setHit(bool ok);
    void alphaDamageEffect(int alpha);
    void resetDamageEffect();

    void drawDamageEffect(sf::RenderWindow& window) const;

    sf::Vector2f getWeaponTipPos() const;
    bool isHit() const;
    bool Jumping() const;
    float getVelocityY() const;
};

#endif