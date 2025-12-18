#include "Player.h"
#include "Map.h"
#include <iostream>
#include <cmath>
#include "Portal.h"
#include "Weapon.h"

Player::Player(const std::string& n, sf::Texture& tex,
    const float posx_, const float posy_,
    std::list<sf::Sound>& activeSounds_,
    const sf::SoundBuffer& jumpSound_,
    std::unique_ptr<Weapon> startingWeapon)
    : Entity(n, posx_, posy_, DEF_SPEED, DEF_GRAVITY, tex, HITBOX_WIDTH, HITBOX_HEIGHT_STANDING),
    velocity{0.f},
    maxJump{DEF_JUMP_FORCE},
    jumpCooldown{0.f},
    weapon{std::move(startingWeapon)},
    animationTimer{0.f},
    frameDuration{0.12f},
    shootingTimer{0.f},
    damageEffectTimer{0.f},
    isRunning{false},
    isJumping{false},
    facingRight{true},
    facingUp{false},
    isHit{false},
    wasRPressedLastFrame{false},
    isCrouching{false},
    frameSize{FRAME_SIZE},
    currentFrame{0},
    animationRow{0},
    animationStartIndex{0},
    animationFrameCount{1},
    activeSounds{activeSounds_},
    jumpSound{jumpSound_}
{
    damageOverlay.setSize(sf::Vector2f(3000.f, 3000.f));
    damageOverlay.setFillColor(sf::Color::Transparent);

    sprite.setPosition(sf::Vector2f(posX, posY));
    sprite.setOrigin(sf::Vector2f(static_cast<float>(FRAME_SIZE.x) / 2.f,
                                  static_cast<float>(FRAME_SIZE.y) + SPRITE_OFFSET_Y));

    sprite.setTextureRect(sf::IntRect(sf::Vector2i(0, 0), FRAME_SIZE));
    updateHitbox();
    jumpSound.setVolume(50.f); // until volume change option
}

void Player::doBehavior(const float deltaTime, const Map& map) {
    if (jumpCooldown > 0.0f) jumpCooldown -= deltaTime;
    if (shootingTimer > 0.0f) shootingTimer -= deltaTime;

    const float oldX = posX;
    const float oldY = posY;

    handleInput(deltaTime, map);
    if (posX != oldX) {
        updateHitbox();
        resolveCollisionX(map, oldX);
    }

    applyGravity(deltaTime);
    if (posY != oldY) {
        updateHitbox();
        resolveCollisionY(map, oldY);
    }

    sprite.setPosition({std::round(posX), std::round(posY)});

    weapon->update(deltaTime, map);
    updateSpriteDirection();
    updateAnimation(deltaTime);
    updateDamageEffect(deltaTime);
}

void Player::handleInput(const float deltaTime, const Map& map) {
    handleCrouchInput(map);
    handleMovementInput(deltaTime);
    handleJumpInput(map);
    handleShootingInput(map);

    const bool isRPressed = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::R);
    if (isRPressed && !wasRPressedLastFrame) {
        weapon->tryReload();
    }
    wasRPressedLastFrame = isRPressed;
}

void Player::handleCrouchInput(const Map& map) {
    bool wantsToCrouch = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S);
    if (isJumping) wantsToCrouch = (shootingTimer <= 0.0f);

    float targetHeight = wantsToCrouch ? HITBOX_HEIGHT_CROUCHING : HITBOX_HEIGHT_STANDING;

    if (!wantsToCrouch && isCrouching) {
        if (checkCeilingCollision(map)) {
            targetHeight = HITBOX_HEIGHT_CROUCHING;
            wantsToCrouch = true;
        }
    }

    isCrouching = wantsToCrouch;

    if (hitboxHeight != static_cast<int>(targetHeight)) {
        hitboxHeight = static_cast<int>(targetHeight);
        updateHitbox();
    }
}

void Player::handleMovementInput(const float deltaTime) {
    const bool movedLeft = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A);
    const bool movedRight = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D);

    isRunning = false;

    if (movedLeft == movedRight) return;
    if (isCrouching && !isJumping) return;

    if (movedLeft) {
        posX -= speed * deltaTime;
        if (shootingTimer <= 0.0f) facingRight = false;
        isRunning = true;
    }
    else {
        posX += speed * deltaTime;
        if (shootingTimer <= 0.0f) facingRight = true;
        isRunning = true;
    }
}

void Player::handleJumpInput(const Map& map) {
    if (isJumping || jumpCooldown > 0.f) return;

    const bool jumpKey = (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W) ||
                          sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Space));

    if (jumpKey) {
        const float testFeetX = posX - static_cast<float>(hitboxWidth) / 2.0f + JUMP_BUFFER_X;
        const sf::FloatRect groundCheck(
            sf::Vector2f(testFeetX, posY),
            sf::Vector2f(static_cast<float>(hitboxWidth) - CEILING_CHECK_OFFSET, GROUND_CHECK_HEIGHT)
        );

        if (map.isWall(groundCheck, true)) {
            velocity = maxJump;
            isJumping = true;
            jumpCooldown = JUMP_COOLDOWN_TIME;
            activeSounds.emplace_back(jumpSound);
            activeSounds.back().play();
        }
    }
}

void Player::handleShootingInput(const Map& map) {
    sf::Vector2f shootDirection;
    bool wantsToShoot = false;

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left)) {
        shootDirection = {-1.f, 0.f};
        wantsToShoot = true;
    } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right)) {
        shootDirection = {1.f, 0.f};
        wantsToShoot = true;
    } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Up)) {
        shootDirection = {0.f, -1.f};
        wantsToShoot = true;
    }

    if (wantsToShoot) {
        if (isJumping && checkCeilingCollision(map)) return;
        fire(shootDirection);
    }
}

void Player::resolveCollisionX(const Map& map, const float lastPosX) {
    if (map.isWall(hitbox, true)) {
        posX = lastPosX;
        updateHitbox();
    }
}

void Player::resolveCollisionY(const Map& map, const float lastPosY) {
    if (map.isWall(hitbox, true)) {
        if (velocity > 0) {
            posY = lastPosY;
            velocity = 0;

            if (checkCeilingCollision(map)) {
                isJumping = true;
                hitboxHeight = static_cast<int>(HITBOX_HEIGHT_CROUCHING);
                isCrouching = true;
            } else {
                isJumping = false;
            }
        }
        else if (velocity < 0) {
            posY = lastPosY;
            velocity = 0;
        }
        updateHitbox();
    }
}

void Player::applyGravity(const float deltaTime) {
    velocity += gravity * deltaTime;
    posY += velocity * deltaTime;
}

void Player::updateAnimation(const float deltaTime) {
    int nextFrameCount;
    if (isJumping) nextFrameCount = 3;
    else if (isRunning && !isCrouching) nextFrameCount = 4;
    else nextFrameCount = 1;

    if (nextFrameCount != animationFrameCount) {
        currentFrame = 0;
        animationTimer = 0.f;
    }

    animationFrameCount = nextFrameCount;

    if (animationFrameCount > 1) {
        animationTimer += deltaTime;
        if (animationTimer >= frameDuration) {
            animationTimer -= frameDuration;
            currentFrame = (currentFrame + 1) % animationFrameCount;
        }
    } else {
        currentFrame = 0;
    }

    sprite.setTextureRect(calculateAnimationRect());
    applySpriteOriginCorrection();
}

sf::IntRect Player::calculateAnimationRect() {
    int col = 0;
    int row;

    if (isJumping) {
        if (shootingTimer > 0.0f) {
            col = 2;
            row = facingUp ? ANIM_ROW_UP : ANIM_ROW_SHOOT;
        } else {
            col = 5;
            row = currentFrame;
        }
    }
    else if (isCrouching) {
        col = ANIM_COLS_CROUCH;
        if (facingUp) row = ANIM_ROW_UP;
        else row = (shootingTimer > 0.0f) ? ANIM_ROW_SHOOT : ANIM_ROW_IDLE;
    }
    else {
        row = (shootingTimer > 0.0f) ? (facingUp ? ANIM_ROW_UP : ANIM_ROW_SHOOT) : ANIM_ROW_IDLE;
        col = isRunning ? currentFrame : 0;
    }

    animationRow = row;
    return {sf::Vector2i(col * FRAME_SIZE.x, row * FRAME_SIZE.y), FRAME_SIZE};
}

void Player::applySpriteOriginCorrection() {
    float yOffset = 0.f;
    if (animationRow == ANIM_ROW_SHOOT) yOffset = 2.5f;
    else if (animationRow == ANIM_ROW_UP) yOffset = 4.5f;

    sprite.setOrigin(sf::Vector2f(static_cast<float>(FRAME_SIZE.x) / 2.f,
                                  static_cast<float>(FRAME_SIZE.y) + SPRITE_OFFSET_Y + yOffset));
}

void Player::updateSpriteDirection() {
    sprite.setScale(sf::Vector2f(facingRight ? 1.f : -1.f, 1.f));
}

void Player::fire(const sf::Vector2f& direction) {
    if (direction.y < 0) facingUp = true;
    else {
        facingUp = false;
        if (direction.x != 0) facingRight = (direction.x > 0);
    }
    updateSpriteDirection();

    if (weapon->tryFire(*this, direction)) shootingTimer = shootingDuration;
}

void Player::takeDamage(const int damageAmount) {
    health -= damageAmount;
    isHit = true;
    damageEffectTimer = damageEffectDuration;
    damageOverlay.setFillColor(sf::Color(255, 0, 0, 150));
}

void Player::updateDamageEffect(const float deltaTime) {
    if (damageEffectTimer > 0.0f) {
        damageEffectTimer -= deltaTime;
        const float ratio = damageEffectTimer / damageEffectDuration;
        const int alpha = static_cast<int>(150 * ratio);
        damageOverlay.setFillColor(sf::Color(255, 0, 0, std::max(0, alpha)));
    } else if (isHit) {
        isHit = false;
        damageOverlay.setFillColor(sf::Color::Transparent);
    }
}

bool Player::checkCeilingCollision(const Map& map) const {
    float checkWidth = static_cast<float>(hitboxWidth) - CEILING_CHECK_OFFSET;
    const sf::FloatRect ceilingCheck(
        {posX - checkWidth / 2.f, posY - HITBOX_HEIGHT_STANDING},
        {checkWidth, HITBOX_HEIGHT_CROUCHING}
    );
    return map.isWall(ceilingCheck, true);
}

sf::Vector2f Player::getWeaponTipPos() const {
    float offsetX = facingUp ? 10.f : static_cast<float>(FRAME_SIZE.x) * 0.25f;
    if (!facingRight) offsetX = facingUp ? -10.f : -static_cast<float>(FRAME_SIZE.x) * 0.3f;

    float offsetY = facingUp ? -(static_cast<float>(FRAME_SIZE.y) * 0.9f)
                             : -(static_cast<float>(FRAME_SIZE.y) * 0.5f);

    if (isCrouching) offsetY += facingUp ? 40.f : 50.f;
    return {posX + offsetX, posY + offsetY};
}

sf::FloatRect Player::doGetBounds() const {
    return hitbox;
}

void Player::checkProjectileCollisions(const std::vector<std::unique_ptr<Entity>>& targets) const {
    weapon->handleCollisions(targets);
}

void Player::resetWeaponProjectiles() const {
    if (weapon) weapon->clearProjectiles();
}

std::unique_ptr<Entity> Player::clone() const {
    return std::make_unique<Player>(*this);
}

Player::~Player() {
    std::cout << "Destructor Player\n";
}

void Player::spawn(const float x, const float y) {
    setPosition(x, y);
    velocity = 0.f;
    isRunning = false;
    isJumping = false;
    isHit = false;
    damageOverlay.setFillColor(sf::Color::Transparent);
    updateHitbox();
}

void Player::resurrect() {
    health = max_health;
    alive = true;
    isHit = false;
    velocity = 0.0f;
    isJumping = false;
    isRunning = false;
    damageEffectTimer = 0.0f;
    damageOverlay.setFillColor(sf::Color::Transparent);
    if (weapon) {
        weapon->clearProjectiles();
        weapon->resetAmmo();
    }
    currentFrame = 0;
    animationTimer = 0.0f;
    facingRight = true;
    updateSpriteDirection();
    updateHitbox();
}

void Player::draw(sf::RenderWindow &window) const {
    if(isAlive()) {
        weapon->draw(window);
        window.draw(sprite);
        if (damageOverlay.getFillColor().a > 0) {
            const sf::View currentView = window.getView();
            window.setView(window.getDefaultView());
            window.draw(damageOverlay);
            window.setView(currentView);
        }
    }
}