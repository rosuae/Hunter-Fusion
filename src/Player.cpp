#include "Player.h"
#include "Map.h"

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

void Player::checkDeath(sf::RenderWindow& window) {
    if (health <= 0) {
        health = 0;
        isalive = false;
        window.close();
        std::cout << "GAME OVER \n";
    }
}

float Player::calculateWeaponOffsetX() const {
    float offsetX = static_cast<float>(frameSize.x) * 0.3f;
    return facingRight ? offsetX : -offsetX;
}

Player::Player(std::string n, sf::Texture& tex):
name{std::move(n)},
health{100},
posX{1000.0f},
posY{450.0f},
speed{900.0f},
gravity{2000.f},
velocity{0.0f},
maxJump{-1000.f},
texture {tex},
sprite{texture},

frameSize{sf::Vector2i(149, 148)},
hitboxWidth{80},
hitboxHeight{80},
animationTimer{0.0f},
frameDuration{0.12f},
shootingTimer{0.0f},

isRunning{false},
isJumping{false},

currentFrame{0},
animationRow{0},
animationStartIndex{0},
animationFrameCount{1}
{   damageOverlay.setSize(sf::Vector2f(3000.f, 3000.f));
    damageOverlay.setFillColor(sf::Color(255, 0, 0, 0));

    sprite.setPosition(sf::Vector2f(posX, posY));
    sprite.setOrigin(sf::Vector2f(static_cast<float>(frameSize.x) / 2.f, static_cast<float>(frameSize.y)));
    sprite.scale(sf::Vector2f(1.f, 1.f));

    sprite.setTextureRect(sf::IntRect(sf::Vector2i(0, 0), frameSize));
}

void Player::PlayerMovement(float deltaTime, std::list<sf::Sound>& sounds, const sf::SoundBuffer& buffer, const Map& map) {

    if ((sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W) ||
         sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Space)) && !isJumping) {
        velocity = maxJump;
        isJumping = true;

        sounds.emplace_back(buffer);
        sounds.back().play();
    }

    float lastPosX = posX;
    bool moved = false;

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A)) {
        posX -= speed * deltaTime;
        facingRight = false;
        moved = true;
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D)) {
        posX += speed * deltaTime;
        facingRight = true;
        moved = true;
    }

    this->isRunning = moved;

    float testTopLeftX = posX - hitboxWidth / 2.0f;
    float testTopLeftY = posY - hitboxHeight;
    sf::FloatRect testBoundsX(sf::Vector2f(testTopLeftX, testTopLeftY),
                            sf::Vector2f(hitboxWidth, hitboxHeight));

    if (map.isWall(testBoundsX)) {
        posX = lastPosX;
    }

    float lastPosY = posY;

    applyGravity(deltaTime);

    testTopLeftX = posX - hitboxWidth / 2.0f;
    testTopLeftY = posY - hitboxHeight;
    sf::FloatRect testBoundsY(sf::Vector2f(testTopLeftX, testTopLeftY),
                            sf::Vector2f(hitboxWidth, hitboxHeight));

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
    sprite.setPosition(sf::Vector2f(posX, posY));
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

void Player::takeDamage (int damageAmount, sf::RenderWindow& window) {
    health -= damageAmount;
    checkDeath(window);
}

void Player::setHit (const bool ok) {
    isHit_ = ok;
}

void Player::alphaDamageEffect(int alpha) {
    damageOverlay.setFillColor(sf::Color(255, 0, 0, alpha));
}

void Player::resetDamageEffect() {
    damageOverlay.setFillColor(sf::Color(255, 0, 0, 0));
}

void Player::draw(sf::RenderWindow& window) const{
    sf::RectangleShape healthbar(sf::Vector2f(100.f, 10.f));

    float healthbarY = posY - static_cast<float>(frameSize.y) - 15.f;

    healthbar.setPosition(sf::Vector2f(posX - 50.f, healthbarY));
    healthbar.setFillColor(sf::Color::Green);
    healthbar.setSize(sf::Vector2f(static_cast<float>(health), 10.f));

    window.draw(healthbar);
    window.draw(sprite);
}

void Player::drawDamageEffect(sf::RenderWindow& window) const{
    const sf::View currentView = window.getView();

    window.setView(window.getDefaultView());
    window.draw(damageOverlay);
    window.setView(currentView);
}

Player::~Player() { std::cout << "S a apelat destructor Player \n";}

sf::Vector2f Player::getPos() const{
    return {posX, posY};
}

sf::Vector2f Player::getWeaponTipPos() const {
    float offsetX = calculateWeaponOffsetX();
    float offsetY = -( static_cast<float>(frameSize.y) * 0.7f);

    return {posX + offsetX, posY + offsetY};
}


sf::FloatRect Player::getBounds () const {
    float left = posX - hitboxWidth / 2.0f;
    float top = posY - hitboxHeight;
    return {sf::Vector2f(left, top), sf::Vector2f(hitboxWidth, hitboxHeight)};
}

bool Player::isHit () const{
    return isHit_;
}

bool Player::Jumping() const {
    return isJumping;
}

bool Player::isAlive() const {
    return isalive;
}