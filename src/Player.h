#ifndef PLAYER_H
#define PLAYER_H

#include "Entity.h"
#include <SFML/Audio.hpp>
#include <list>

class Map;

class Player : public Entity {
    float velocity;
    float maxJump;

    float animationTimer;
    float frameDuration;
    float shootingTimer;
    static constexpr float shootingDuration = 0.3f;
    float damageEffectTimer;

    bool isRunning;
    bool isJumping;
    bool facingRight;
    bool facingUp;
    bool isHit;

    sf::Vector2i frameSize;
    int currentFrame;
    int animationRow;
    int animationStartIndex;
    int animationFrameCount;
    sf::RectangleShape damageOverlay;

    std::list<sf::Sound>& activeSounds;
    sf::Sound jumpSound;

    sf::FloatRect doGetBounds() const override;
    void takeDamage(int damageAmount) override;
    void doBehavior(float deltaTime, const Map&) override;
    void applyGravity(float deltaTime) override;
    void updateSpriteDirection();
    void updateAnimation(float deltaTime);
    void handleInput (float deltaTime, const Map& map);
    void updateDamageEffect(float deltaTime);

public:
    Player(const std::string& n, sf::Texture& tex, float posx_, float posy_, std::list<sf::Sound>& activeSounds_, const sf::SoundBuffer& jumpSound_);
    std::unique_ptr<Entity> clone() const override;
    ~Player() override;

    void spawn(float x, float y);
    void draw(sf::RenderWindow& window) const override;
    void shoot(const sf::Vector2f& direction);

    sf::Vector2f getWeaponTipPos() const;
    bool hitAffected() const;
    [[nodiscard]]int getHealth() const { return health; }
    [[nodiscard]]bool canAttack() const;
};

#endif