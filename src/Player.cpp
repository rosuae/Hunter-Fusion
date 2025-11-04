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
    return facingRight ? 30.f : -30.f;
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
isJumping{false},
texture {tex},
sprite{texture}
{   damageOverlay.setSize(sf::Vector2f(3000.f, 3000.f));
    damageOverlay.setFillColor(sf::Color(255, 0, 0, 0));

    sprite.setPosition(sf::Vector2f(posX, posY));
    sprite.setOrigin(sf::Vector2f(static_cast<float>(texture.getSize().x) / 2.f, static_cast<float>(texture.getSize().y)));
    sprite.scale(sf::Vector2f(1.f, 1.f));
    std::cout<<"Constructor Player \n";
}

void Player::PlayerMovement(float deltaTime, std::list<sf::Sound>& sounds, const sf::SoundBuffer& buffer, const Map& map) {

    if ((sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W) ||
         sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Space)) && !isJumping) {
        velocity = maxJump;
        isJumping = true;

        sounds.emplace_back(buffer);
        sounds.back().play();
    }

    sf::FloatRect localBounds = sprite.getLocalBounds();
    float spriteWidth = localBounds.size.x;
    float spriteHeight = localBounds.size.y;

    float lastPosX = posX;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A)) {
        posX -= speed * deltaTime;
        facingRight = false;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D)) {
        posX += speed * deltaTime;
        facingRight = true;
    }

    float testTopLeftX = posX - spriteWidth / 2.0f;
    float testTopLeftY = posY - spriteHeight;

    sf::FloatRect testBoundsX(sf::Vector2f(testTopLeftX, testTopLeftY),
                            sf::Vector2f(spriteWidth, spriteHeight));

    if (map.isWall(testBoundsX)) {
        posX = lastPosX;
    }

    float lastPosY = posY;

    isJumping = true;

    applyGravity(deltaTime);

    testTopLeftX = posX - spriteWidth / 2.0f;
    testTopLeftY = posY - spriteHeight;

    sf::FloatRect testBoundsY(sf::Vector2f(testTopLeftX, testTopLeftY),
                            sf::Vector2f(spriteWidth, spriteHeight));

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

void Player::takeDamage (int damageAmount, sf::RenderWindow& window) {
    health -= damageAmount;
    checkDeath(window);
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