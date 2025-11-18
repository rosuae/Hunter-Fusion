#include "Player.h"
#include "Map.h"
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

float Player::calculateWeaponOffsetX() const {
    float offsetX = static_cast<float>(frameSize.x) * 0.3f;
    return facingRight ? offsetX : -offsetX;
}

Player::Player(const std::string& n, sf::Texture& tex, std::list<sf::Sound>& activeSounds_, const sf::SoundBuffer& jumpSound_)
    : Entity(n, 1000.0f, 450.0f, 900.0f, 2000.f, tex),
      velocity{0.0f},
      maxJump{-1000.f},
      frameSize{sf::Vector2i(149, 148)},
      hitboxWidth{80},
      hitboxHeight{80},
      animationTimer{0.0f},
      frameDuration{0.12f},
      shootingTimer{0.0f},
      isRunning{false},
      isJumping{false},
      facingRight{true},
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
                                   static_cast<float>(frameSize.y))
                                   );
    sprite.scale(sf::Vector2f(1.f, 1.f));
    sprite.setTextureRect(sf::IntRect(sf::Vector2i(0, 0), frameSize));
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
        velocity = maxJump;
        isJumping = true;

        activeSounds.emplace_back(jumpSound);
        activeSounds.back().play();
    }

    float lastPosX = posX;
    bool moved = false;

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A)) {
        posX -= speed * deltaTime;
        if (shootingTimer <= 0.0f) {
            facingRight = false;
        }
        moved = true;
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D)) {
        posX += speed * deltaTime;
        if (shootingTimer <= 0.0f) {
            facingRight = true;
        }
        moved = true;
    }

    this->isRunning = moved;

    float testTopLeftX = posX - static_cast<float>(hitboxWidth) / 2.0f;
    float testTopLeftY = posY - static_cast<float>(hitboxHeight);
    sf::FloatRect testBoundsX(sf::Vector2f(testTopLeftX, testTopLeftY),
                            sf::Vector2f(static_cast<float>(hitboxWidth),
                                static_cast<float>(hitboxHeight))
                                );

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
        posY = lastPosY;

        if (velocity > 0) {
            isJumping = false;
            velocity = 0;
        } else if (velocity < 0) {
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
        int jumpCol = 5;

        if (animationStartIndex != jumpCol) {
            int jumpFrameCount = 2;
            animationStartIndex = jumpCol;
            animationFrameCount = jumpFrameCount;
            currentFrame = 0;
            animationTimer = 0.0f;
        }

        animationTimer += deltaTime;
        if (animationTimer >= frameDuration) {
            animationTimer -= frameDuration;
            currentFrame = (currentFrame + 1) % animationFrameCount;
        }

        int rectLeft = jumpCol * frameSize.x;
        int rectTop = currentFrame * frameSize.y;
        sprite.setTextureRect(sf::IntRect(sf::Vector2i(rectLeft, rectTop), frameSize));

        return;
    }

    if (shootingTimer > 0.0f) {
        animationRow = 1;
    } else {
        animationRow = 0;
    }

    int newStartIndex;
    int newFrameCount;
    bool loopAnimation;

    if (isRunning) {
        newStartIndex = 2;
        newFrameCount = 3;
        loopAnimation = true;
    }
    else if (shootingTimer > 0.0f) {
        newStartIndex = 1;
        newFrameCount = 1;
        loopAnimation = false;
    }
    else {
        newStartIndex = 0;
        newFrameCount = 1;
        loopAnimation = true;
    }

    if (newStartIndex != animationStartIndex) {
        animationStartIndex = newStartIndex;
        animationFrameCount = newFrameCount;
        currentFrame = 0;
        animationTimer = 0.0f;
    }

    animationTimer += deltaTime;
    if (animationTimer >= frameDuration) {
        animationTimer -= frameDuration;
        if (loopAnimation) {
            currentFrame = (currentFrame + 1) % animationFrameCount;
        } else {
            if (currentFrame < animationFrameCount - 1) {
                currentFrame++;
            }
        }
    }

    int rectLeft = (animationStartIndex + currentFrame) * frameSize.x;
    int rectTop = animationRow * frameSize.y;
    sprite.setTextureRect(sf::IntRect(sf::Vector2i(rectLeft, rectTop), frameSize));
}

void Player::shootAnimation() {
    shootingTimer = shootingDuration;
}

void Player::setFacing(bool isFacingRight) {
    facingRight = isFacingRight;
    updateSpriteDirection();
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
    sf::RectangleShape healthbar(sf::Vector2f(100.f, 5.f));

    float healthbarY = posY - static_cast<float>(frameSize.y) - 15.f;

    healthbar.setPosition(sf::Vector2f(posX - 50.f, healthbarY));
    healthbar.setFillColor(sf::Color::Green);
    healthbar.setSize(sf::Vector2f(static_cast<float>(health), 5.f));

    window.draw(healthbar);
    window.draw(sprite);
}

void Player::drawDamageEffect(sf::RenderWindow& window) const {
    const sf::View currentView = window.getView();

    window.setView(window.getDefaultView());
    window.draw(damageOverlay);
    window.setView(currentView);
}


sf::Vector2f Player::getWeaponTipPos() const {
    float offsetX = calculateWeaponOffsetX();
    float offsetY = -(static_cast<float>(frameSize.y) * 0.6f);

    return {posX + offsetX, posY + offsetY};
}

sf::FloatRect Player::doGetBounds() const {
    float left = posX - static_cast<float>(hitboxWidth) / 2.0f;
    float top = posY - static_cast<float>(hitboxHeight);
    return {sf::Vector2f(left, top),
                            sf::Vector2f(
                            static_cast<float>(hitboxWidth),
                                static_cast<float>(hitboxHeight))
                };
}

bool Player::isHit() const {
    return isHit_;
}

bool Player::Jumping() const {
    return isJumping;
}