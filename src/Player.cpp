#include "Player.h"
#include "Map.h"
#include "GameExceptions.h"
#include <iostream>

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

Player::Player(const std::string& n, sf::Texture& tex, float posx_, float posy_, std::list<sf::Sound>& activeSounds_, const sf::SoundBuffer& jumpSound_)
    : Entity(n, posx_, posy_, 900.0f, 2500.f, tex),
      velocity{0.0f},
      maxJump{-1450.f},
      frameSize{sf::Vector2i(224, 222)},
      hitboxWidth{120},
      hitboxHeight{120},
      animationTimer{0.0f},
      frameDuration{0.12f},
      shootingTimer{0.0f},
      isRunning{false},
      isJumping{false},
      facingRight{true},
      facingUp{false},
      isHit_{false},
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

    jumpSound.setVolume(50); //until volume settings feature
}

std::unique_ptr<Entity> Player::clone() const {
    return std::make_unique<Player>(*this);
}

Player::~Player() {
    std::cout << "S a apelat destructor Player \n";
}
void Player::doBehavior(float deltaTime, const Map& map) {

    if ((sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W) ||
         sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Space)) && !isJumping) {
            try {
                float testFeetX = posX - static_cast<float>(hitboxWidth) / 2.0f + 10.f;
                float testFeetY = posY;

                sf::FloatRect groundCheck(
                    sf::Vector2f(testFeetX, testFeetY),
                    sf::Vector2f(static_cast<float>(hitboxWidth) - 20.f, 10.f)
                );

                if (!map.isWall(groundCheck)) {
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
    bool movedLeft = false;
    bool movedRight = false;

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A)) {
        movedLeft = true;
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D)) {
        movedRight = true;
    }

    if (movedLeft && !movedRight) {
        posX -= speed * deltaTime;
        if (shootingTimer <= 0.0f) {
            facingRight = false;
        }
        this->isRunning = true;
    } else if (movedRight && !movedLeft) {
        posX += speed * deltaTime;
        if (shootingTimer <= 0.0f) {
            facingRight = true;
        }
        this->isRunning = true;
    } else {
        this->isRunning = false;
    }

    float testTopLeftX = posX - static_cast<float>(hitboxWidth) / 2.0f;
    float testTopLeftY = posY - static_cast<float>(hitboxHeight);
    sf::FloatRect testBoundsX(sf::Vector2f(testTopLeftX, testTopLeftY),
                            sf::Vector2f(static_cast<float>(hitboxWidth),
                                static_cast<float>(hitboxHeight)));

    if (map.isWall(testBoundsX)) {
        posX = lastPosX;
    }

    float lastPosY = posY;

    applyGravity(deltaTime);

    testTopLeftX = posX - static_cast<float>(hitboxWidth) / 2.0f;
    testTopLeftY = posY - static_cast<float>(hitboxHeight);
    sf::FloatRect testBoundsY(sf::Vector2f(testTopLeftX, testTopLeftY),
                            sf::Vector2f(static_cast<float>(hitboxWidth),
                                static_cast<float>(hitboxHeight))
                                );

    if (map.isWall(testBoundsY)) {
        if (velocity > 0) {

            posY = lastPosY;
            velocity = 0;
            isJumping = false;
        }
        else if (velocity < 0) {
            posY = lastPosY;
            velocity = 0;
        }
    }

    updateSpriteDirection();
    updateAnimation(deltaTime);
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

    float yOffset = 0.f;
    if (animationRow == 1) {
        yOffset = 2.5f;
    }
    else if (animationRow == 2) {
        yOffset = 4.5f;
    }
    sprite.setOrigin(sf::Vector2f(originBaseX, originBaseY + yOffset));
}

void Player::shootAnimation() {
    shootingTimer = shootingDuration;
}

void Player::setFacing(bool isFacingRight) {
    facingRight = isFacingRight;
    updateSpriteDirection();
}

void Player::setFacingUp(bool isFacingUp) {
    facingUp = isFacingUp;
}

void Player::doTakeDamage(int damageAmount) {
    health -= damageAmount;
}

void Player::setHit(const bool ok) {
    isHit_ = ok;
}

void Player::alphaDamageEffect(int alpha) {
    damageOverlay.setFillColor(sf::Color(255, 0, 0, alpha));
}

void Player::resetDamageEffect() {
    damageOverlay.setFillColor(sf::Color(255, 0, 0, 0));
}

void Player::doDraw(sf::RenderWindow& window) const {
    window.draw(sprite);
}

void Player::drawDamageEffect(sf::RenderWindow& window) const {
    const sf::View currentView = window.getView();

    window.setView(window.getDefaultView());
    window.draw(damageOverlay);
    window.setView(currentView);
}


sf::Vector2f Player::getWeaponTipPos() const {
    float offsetX = 0.f;
    float offsetY = 0.f;

    if (facingRight) {
        if (facingUp) offsetX = 10.f;
        else offsetX = static_cast<float>(frameSize.x) * 0.3f;
    }
    else {
        if (facingUp) offsetX = -10.f;
        else offsetX = -static_cast<float>(frameSize.x) * 0.3f;
    }

    if (facingUp) {
        offsetY = -(static_cast<float>(frameSize.y) * 0.9f);
    } else {
        offsetY = -(static_cast<float>(frameSize.y) * 0.5f);
    }

    return {posX + offsetX, posY + offsetY};
}

sf::FloatRect Player::doGetBounds() const {
    float left = posX - static_cast<float>(hitboxWidth) / 2.0f;
    float top = posY - static_cast<float>(hitboxHeight);
    return {sf::Vector2f(left, top),
                            sf::Vector2f(
                            static_cast<float>(hitboxWidth),
                                static_cast<float>(hitboxHeight))};
}

bool Player::isHit() const {
    return isHit_;
}

bool Player::Jumping() const {
    return isJumping;
}

float Player::getVelocityY() const {
    return velocity;
}