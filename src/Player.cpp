#include "Player.h"
#include "Map.h"
#include "GameExceptions.h"
#include <iostream>
#include "Portal.h"
#include "Weapon.h"

void Player::updateSpriteDirection() {
    if (facingRight)
        sprite.setScale(sf::Vector2f(1.f, 1.f));
    else
        sprite.setScale(sf::Vector2f(-1.f, 1.f));
}

void Player::applyGravity(float deltaTime) {
    velocity += gravity * deltaTime;
    posY += velocity * deltaTime;
}

Player::Player(const std::string& n, sf::Texture& tex,
    float posx_, float posy_,
    std::list<sf::Sound>& activeSounds_,
    const sf::SoundBuffer& jumpSound_,
    std::unique_ptr<Weapon> startingWeapon)
    : Entity(n, posx_, posy_, 600.0f, 2500.f, tex, 120, 120),
    velocity{0.0f},
    maxJump{-1450.f},
    weapon{std::move(startingWeapon)},
    animationTimer{0.0f},
    frameDuration{0.12f},
    shootingTimer{0.0f},
    damageEffectTimer{0.0f},
    isRunning{false},
    isJumping{false},
    facingRight{true},
    facingUp{false},
    isHit{false},
    frameSize{sf::Vector2i(224, 222)},
    currentFrame{0},
    animationRow{0},
    animationStartIndex{0},
    animationFrameCount{1},
    activeSounds{activeSounds_},
    jumpSound{jumpSound_}
{
    damageOverlay.setSize(sf::Vector2f(3000.f, 3000.f));
    damageOverlay.setFillColor(sf::Color(255, 0, 0, 0));

    sprite.setPosition(sf::Vector2f(posX, posY));
    sprite.setOrigin(sf::Vector2f(static_cast<float>(frameSize.x) / 2.f,
                                  (static_cast<float>(frameSize.y)) - 7.f)
                                   );
    sprite.scale(sf::Vector2f(1.f, 1.f));
    sprite.setTextureRect(sf::IntRect(sf::Vector2i(0, 0), frameSize));
    updateHitbox();
    jumpSound.setVolume(50); //until volume settings feature
}

std::unique_ptr<Entity> Player::clone() const {
    return std::make_unique<Player>(*this);
}

Player::~Player() {
    std::cout << "S a apelat destructor Player \n";
}

void Player::doBehavior(float deltaTime, const Map& map) {
    handleInput(deltaTime, map);

    const float lastPosY = posY;
    applyGravity(deltaTime);

    if (posY != lastPosY) {
        updateHitbox();

        if (map.isWall(hitbox, true)) {
            if (velocity > 0) {
                posY = lastPosY;
                velocity = 0;
                isJumping = false;
            }
            else if (velocity < 0) {
                posY = lastPosY;
                velocity = 0;
            }
            updateHitbox();
        }
    }

    weapon->update(deltaTime, map);
    updateSpriteDirection();
    updateAnimation(deltaTime);
    updateDamageEffect(deltaTime);
}

void Player::handleInput(float deltaTime, const Map& map) {
    if ((sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W) ||
         sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Space)) && !isJumping) {
        try {
            float testFeetX = posX - static_cast<float>(hitboxWidth) / 2.0f + 10.f;
            float testFeetY = posY;

            sf::FloatRect groundCheck(
                sf::Vector2f(testFeetX, testFeetY),
                sf::Vector2f(static_cast<float>(hitboxWidth) - 20.f, 10.f)
            );

            if (!map.isWall(groundCheck, true)) {
                throw InvalidActionException("Jump", "Player is in mid air (falling)");
            }

            velocity = maxJump;
            isJumping = true;

            activeSounds.emplace_back(jumpSound);
            activeSounds.back().play();
        }
        catch (const InvalidActionException& e) {
            std::cout << e.what() << std::endl;
        }
    }

    float lastPosX = posX;
    bool movedLeft = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A);
    bool movedRight = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D);

    isRunning = false;

    if (movedLeft && !movedRight) {
        posX -= speed * deltaTime;
        if (shootingTimer <= 0.0f) {
            facingRight = false;
        }
        isRunning = true;
    }
    else if (movedRight && !movedLeft) {
        posX += speed * deltaTime;
        if (shootingTimer <= 0.0f) {
            facingRight = true;
        }
        isRunning = true;
    }

    if (posX != lastPosX) {
        updateHitbox();
        if (map.isWall(hitbox, true)) {
            posX = lastPosX;
            updateHitbox();
        }
    }
}

void Player::spawn(const float x, const float y) {
    setPosition(x, y);
    velocity = 0.f;
    isRunning = false;
    isJumping = false;
    isHit = false;
    damageOverlay.setFillColor(sf::Color(255, 0, 0, 0));
    updateHitbox();
}

void Player::updateAnimation(float deltaTime) {
    if (shootingTimer > 0.0f) {
        shootingTimer -= deltaTime;
    }
    if (isJumping) {
        animationFrameCount = 3;
        if (shootingTimer > 0.0f) {
            int col = 2;
            int row = facingUp ? 2 : 1;
            int rectLeft = col * frameSize.x;
            int rectTop = row * frameSize.y;
            sprite.setTextureRect(sf::IntRect(sf::Vector2i(rectLeft, rectTop), frameSize));
            return;
        }

        animationTimer += deltaTime;

        if (animationTimer >= frameDuration) {
            animationTimer -= frameDuration;
            currentFrame = (currentFrame + 1) % animationFrameCount;
        }
        if (currentFrame >= animationFrameCount) {
            currentFrame = 0;
        }

        int col = 5;
        int row = currentFrame;

        int rectLeft = col * frameSize.x;
        int rectTop = row * frameSize.y;
        sprite.setTextureRect(sf::IntRect(sf::Vector2i(rectLeft, rectTop), frameSize));
        return;
    }

    if (shootingTimer > 0.0f) {
        animationRow = facingUp ? 2 : 1;
    } else {
        animationRow = 0;
    }
    animationStartIndex = 0;
    animationFrameCount = 4;

    if (isRunning) {
        if (currentFrame == 0 && animationTimer == 0.0f) {
            currentFrame = 1;
        }
        animationTimer += deltaTime;
        if (animationTimer >= frameDuration) {
            animationTimer -= frameDuration;
            currentFrame = (currentFrame + 1) % animationFrameCount;
        }
    } else {
        currentFrame = 0;
        animationTimer = frameDuration;
    }

    int col = animationStartIndex + currentFrame;
    int row = animationRow;

    int rectLeft = col * frameSize.x;
    int rectTop = row * frameSize.y;

    sprite.setTextureRect(sf::IntRect(sf::Vector2i(rectLeft, rectTop), frameSize));

    float originBaseY = static_cast<float>(frameSize.y) - 7.f;
    float originBaseX = static_cast<float>(frameSize.x) / 2.f;

    float yOffset = animationRow == 1 ? 2.5f : animationRow == 2 ? 4.5f : 0.f;
    sprite.setOrigin(sf::Vector2f(originBaseX, originBaseY + yOffset));
}

void Player::fire(const sf::Vector2f& direction) {
    if (weapon->fire(*this, direction)) {
        shootingTimer = shootingDuration;

        if (direction.y < 0) facingUp = true;
        else {
            facingUp = false;
            facingRight = (direction.x > 0);
            updateSpriteDirection();
        }
    }
}

void Player::reload() const {
    weapon->reload();
}

void Player::checkProjectileCollisions(const std::vector<std::unique_ptr<Entity>>& targets) const {
    weapon->handleCollisions(targets);
}

void Player::takeDamage(int damageAmount) {
    health -= damageAmount;
    isHit = true;
    damageEffectTimer = 0.3f;
    damageOverlay.setFillColor(sf::Color(255, 0, 0, 150));
}

void Player::updateDamageEffect(float deltaTime) {
    if (damageEffectTimer > 0.0f) {
        damageEffectTimer -= deltaTime;

        const float ratio = damageEffectTimer / 0.3f;
        int alpha = static_cast<int>(150 * ratio);
        if (alpha < 0) alpha = 0;

        damageOverlay.setFillColor(sf::Color(255, 0, 0, alpha));
    }else {
        if (hitAffected()){
            isHit = false;
            damageOverlay.setFillColor(sf::Color(255,0, 0, 0));
        }
    }
}

void Player::draw(sf::RenderWindow &window) const {
    if(isAlive()) {
        weapon->draw(window);
        window.draw(sprite);
        if (damageOverlay.getFillColor().a > 0) {
            sf::View currentView = window.getView();
            window.setView(window.getDefaultView());
            window.draw(damageOverlay);
            window.setView(currentView);
        }
    }
}

sf::Vector2f Player::getWeaponTipPos() const {
    const float offsetX = facingRight ? (facingUp ? 10.f : frameSize.x * 0.25f)
                              : (facingUp ? -10.f : -frameSize.x * 0.3f);
    const float offsetY = facingUp ? -(frameSize.y * 0.9f) : -(frameSize.y * 0.5f);
    return {posX + offsetX, posY + offsetY};
}

sf::FloatRect Player::doGetBounds() const {
    return hitbox;
}

bool Player::hitAffected() const {
    return isHit;
}

bool Player::canAttack() const {
    return !isJumping || velocity > 0;
}