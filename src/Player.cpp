#include "Player.h"

void Player::updateSpriteDirection() {
    if (facingRight)
        sprite.setScale(sf::Vector2f(.5f, .5f));
    else
        sprite.setScale(sf::Vector2f(-.5f, .5f));
}

void Player::applyGravity(float deltaTime) {
    velocity += gravity * deltaTime;
    posY += velocity * deltaTime;
}

void Player::handleGroundCollision() {
    if (posY >= 1000.0f) {
        posY = 1000.f;
        velocity = 0.f;
        isJumping = false;
    }
}

void Player::handleScreenBarriers() {
    if (posX < 0.0f)
        posX = 1920.0f;
    if (posX > 1920.0f)
        posX = 0.0f;
    if (posY < 0.0f)
        posY = 0.0f;
}

void Player::checkDeath() {
    if (health <= 0) {
        health = 0;
        isalive = false;
        std::cout << "GAME OVER \n";
    }
}

float Player::calculateWeaponOffsetX() const {
    return facingRight ? 30.f : -30.f;
}

Player::Player(std::string n, const std::string& texturePath):
    name{std::move(n)},
    health{100},
    posX{900.0f},
    posY{900.0f},
    speed{900.0f},
    gravity{2000.f},
    velocity{0.0f},
    maxJump{-1000.f},
    isJumping{false},
    texture {texturePath},
    sprite {texture}
    {   if (!texture.loadFromFile(texturePath))
            std::cout << "Eroare la incarcarea texturii PLayer";

        damageOverlay.setSize(sf::Vector2f(1920.f, 1080.f));
        damageOverlay.setFillColor(sf::Color(255, 0, 0, 0));

        sprite.setPosition(sf::Vector2f(posX, posY));
        sprite.setOrigin(sf::Vector2f(static_cast<float>(texture.getSize().x) / 2.f, static_cast<float>(texture.getSize().y)));
        sprite.scale(sf::Vector2f(.5f, .5f));
        std::cout<<"Constructor Player \n";
}

void Player::PlayerMovement(float deltaTime, std::list<sf::Sound>& sounds, const sf::SoundBuffer& buffer) {

    if ((sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W) ||
         sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Space)) && !isJumping) {
        velocity = maxJump;
        isJumping = true;

        sounds.emplace_back(buffer);
        sounds.back().play();
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S)) {
        posY += speed * deltaTime;
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A)) {
        posX -= speed * deltaTime;
        facingRight = false;
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D)) {
        posX += speed * deltaTime;
        facingRight = true;
    }

    applyGravity(deltaTime);
    handleGroundCollision();
    handleScreenBarriers();
    updateSpriteDirection();

    sprite.setPosition(sf::Vector2f(posX, posY));
}

void Player::takeDamage (int damageAmount) {
    health -= damageAmount;
    checkDeath();
    if (isalive) {
        std::cout << "Player ul a primit " << damageAmount << " dmg, ramanand cu " << health << " viata \n";
    }
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

    healthbar.setPosition(sf::Vector2f(posX - 50.f, posY - 250.f));
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
    float offsetY = -180.f;

    return {posX + offsetX, posY + offsetY};
}


sf::FloatRect Player::getBounds () const {
    return sprite.getGlobalBounds();
}

bool Player::isHit () const{
    return isHit_;
}

bool Player::isAlive() const {
    return isalive;
}