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
    velocity{0.f, 0.f},
    maxJump{DEF_JUMP_FORCE},
    jumpCooldown{0.f},
    weapon{std::move(startingWeapon)},
    animationTimer{0.f},
    idleTimer{0.f},
    frameDuration{0.12f},
    shootingTimer{0.f},
    damageEffectTimer{0.f},
    isDodging{false},
    dodgeTimer{0.f},
    dodgeCooldownTimer{0.f},
    storedDodgeDir{0.f},
    isRunning{false},
    isJumping{false},
    facingRight{true},
    facingUp{false},
    isHit{false},
    wasRPressedLastFrame{false},
    isCrouching{false},
    currentFrame{0},
    animationRow{0},
    animationStartIndex{0},
    animationFrameCount{1},
    activeSounds{activeSounds_},
    jumpSound{jumpSound_}
{
    constexpr int cols = 6;
    constexpr int rows = 5;

    m_frameSize.x = static_cast<int>(tex.getSize().x) / cols;
    m_frameSize.y = static_cast<int>(tex.getSize().y) / rows;

    damageOverlay.setSize(sf::Vector2f(3000.f, 3000.f));
    damageOverlay.setFillColor(sf::Color::Transparent);

    sprite.setPosition(sf::Vector2f(posX, posY));
    sprite.setOrigin(sf::Vector2f(static_cast<float>(m_frameSize.x) / 2.f,
                                  static_cast<float>(m_frameSize.y)));

    sprite.setTextureRect(sf::IntRect(sf::Vector2i(0, 0), m_frameSize));
    updateHitbox();
    jumpSound.setVolume(25.f); // until volume change option
}

void Player::doBehavior(const float deltaTime, const Map& map) {
    if (jumpCooldown > 0.0f) jumpCooldown -= deltaTime;
    if (shootingTimer > 0.0f) shootingTimer -= deltaTime;
    if (dodgeCooldownTimer > 0.0f) dodgeCooldownTimer -= deltaTime;

    const float oldX = posX;
    const float oldY = posY;

    handleInput(deltaTime, map);

    if (isDodging) {
        dodgeTimer -= deltaTime;
        velocity.x = storedDodgeDir * DODGE_SPEED;

        if (dodgeTimer <= 0.0f) {
            isDodging = false;
            velocity.x = 0.f;

            if (checkCeilingCollision(map)) {
                isCrouching = true;
            }
        }
    }

    posX += velocity.x * deltaTime;
    if (std::abs(posX - oldX) > 0.001f) {
        updateHitbox();
        resolveCollisionX(map, oldX);
    }

    applyGravity(deltaTime);
    handleVariableJumpHeight(deltaTime);
    posY += velocity.y * deltaTime;

    if (std::abs(posY - oldY) > 0.001f) {
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

void Player::handleVariableJumpHeight(const float deltaTime) {
    const bool jumpHeld = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W) ||
                           sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Space);

    if (!jumpHeld && velocity.y < 0) {
        velocity.y += gravity * 2.0f * deltaTime;
    }
}

void Player::handleCrouchInput(const Map& map) {
    const bool isPressingS = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S);

    if (isPressingS && !isDodging && dodgeCooldownTimer <= 0.0f && !isJumping) {
        if (std::abs(velocity.x) > 40.f) {
            startDodge();
            return;
        }
    }

    bool wantsToCrouch = isPressingS;

    if (wantsToCrouch && dodgeCooldownTimer > 0.0f) {
        const bool isTryingToRun = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A) ||
                                   sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D);

        if (isTryingToRun) {
            wantsToCrouch = false;
        }
    }

    if (isJumping) wantsToCrouch = shootingTimer <= 0.0f;

    float targetHeight = wantsToCrouch ? HITBOX_HEIGHT_CROUCHING : HITBOX_HEIGHT_STANDING;

    if (isDodging) {
        wantsToCrouch = true;
        targetHeight = HITBOX_HEIGHT_CROUCHING;
    }
    else if (!wantsToCrouch && isCrouching) {
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

void Player::startDodge() {
    isDodging = true;
    dodgeTimer = DODGE_DURATION;
    dodgeCooldownTimer = DODGE_COOLDOWN_TIME;

    const bool holdingLeft = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A);
    const bool holdingRight = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D);

    if (holdingLeft && !holdingRight) {
        storedDodgeDir = -1.f;
        facingRight = false;
    }
    else if (holdingRight && !holdingLeft) {
        storedDodgeDir = 1.f;
        facingRight = true;
    }
    else {
        storedDodgeDir = facingRight ? 1.f : -1.f;
    }

    velocity.y = 0.f;
}

void Player::handleMovementInput(const float deltaTime) {
    if (isDodging) return;

    const bool movedLeft = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A);
    const bool movedRight = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D);

    isRunning = false;
    if (movedLeft && !movedRight && (!isCrouching || isJumping)) {
        velocity.x -= ACCELERATION * deltaTime;
        if (shootingTimer <= 0.0f) facingRight = false;
    }
    else if (movedRight && !movedLeft && (!isCrouching || isJumping)) {
        velocity.x += ACCELERATION * deltaTime;
        if (shootingTimer <= 0.0f) facingRight = true;
    }
    else {
        if (velocity.x > 0) {
            velocity.x -= DECELERATION * deltaTime;
            if (velocity.x < 0) velocity.x = 0;
        }
        else if (velocity.x < 0) {
            velocity.x += DECELERATION * deltaTime;
            if (velocity.x > 0) velocity.x = 0;
        }
    }

    if (velocity.x > speed) velocity.x = speed;
    if (velocity.x < -speed) velocity.x = -speed;

    if (std::abs(velocity.x) > 10.f) isRunning = true;
}

void Player::handleJumpInput(const Map& map) {
    if (isJumping || jumpCooldown > 0.f) return;

    const bool jumpKey = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W) ||
                          sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Space);

    if (jumpKey) {
        const float testFeetX = posX - static_cast<float>(hitboxWidth) / 2.0f + JUMP_BUFFER_X;
        const sf::FloatRect groundCheck(
            sf::Vector2f(testFeetX, posY),
            sf::Vector2f(static_cast<float>(hitboxWidth) - CEILING_CHECK_OFFSET, GROUND_CHECK_HEIGHT)
        );

        if (map.isWall(groundCheck, true)) {
            velocity.y = maxJump;
            isJumping = true;
            jumpCooldown = JUMP_COOLDOWN_TIME;
            activeSounds.emplace_back(jumpSound);
            activeSounds.back().play();
        }
    }
}

void Player::handleShootingInput(const Map& map) {
    if (isDodging) return;
    sf::Vector2f shootDirection;
    bool wantsToShoot = false;

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left)) {
        shootDirection = {-1.f, 0.f};
        wantsToShoot = true;
    } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right)) {
        shootDirection = {1.f, 0.f};
        wantsToShoot = true;
    } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Up)) {
        if (!isCrouching || isJumping) {
            shootDirection = {0.f, -1.f};
            wantsToShoot = true;
        }
    }

    if (wantsToShoot) {
        if (isJumping && checkCeilingCollision(map)) return;
        fire(shootDirection);
    }
}

void Player::resolveCollisionX(const Map& map, const float lastPosX) {
    if (map.isWall(hitbox, true)) {
        posX = lastPosX;
        velocity.x = 0;
        updateHitbox();
    }
}

void Player::resolveCollisionY(const Map& map, const float lastPosY) {
    if (map.isWall(hitbox, true)) {
        if (velocity.y > 0) {
            posY = lastPosY;
            velocity.y = 0;

            if (checkCeilingCollision(map)) {
                isJumping = true;
                hitboxHeight = static_cast<int>(HITBOX_HEIGHT_CROUCHING);
                isCrouching = true;
            } else {
                isJumping = false;
            }
        }
        else if (velocity.y < 0) {
            posY = lastPosY;
            velocity.y = 0;
        }
        updateHitbox();
    }
}

void Player::applyGravity(const float deltaTime) {
    velocity.y += gravity * deltaTime;
}

void Player::updateAnimation(const float deltaTime) {
    int nextFrameCount;

    if (isJumping || isDodging) {
        nextFrameCount = 4;
    }
    else if (isRunning && !isCrouching) {
        nextFrameCount = 4;
    }
    else {
        nextFrameCount = 1;
    }

    if (nextFrameCount != animationFrameCount) {
        currentFrame = 0;
        animationTimer = 0.f;
    }

    animationFrameCount = nextFrameCount;

    if (animationFrameCount > 1) {
        idleTimer = 0.f;

        animationTimer += deltaTime;
        if (animationTimer >= frameDuration) {
            animationTimer -= frameDuration;
            currentFrame = (currentFrame + 1) % animationFrameCount;
        }
    }
    else {
        currentFrame = 0;
        if (isCrouching || shootingTimer > 0.0f) {
            idleTimer = 0.f;
        }
        else {
            idleTimer += deltaTime;
        }
    }

    sprite.setTextureRect(calculateAnimationRect());
    applySpriteOriginCorrection();
}

sf::IntRect Player::calculateAnimationRect() {
    if (isDodging) {
        constexpr int baseCol = 5;
        constexpr int baseRow = 0;

        int dodgeW = m_frameSize.x / 2;
        int dodgeH = m_frameSize.y / 2;

        const int subCol = currentFrame % 2;
        const int subRow = currentFrame / 2;

        int rectLeft = baseCol * m_frameSize.x + subCol * dodgeW;
        int rectTop  = baseRow * m_frameSize.y + subRow * dodgeH;

        return sf::IntRect({rectLeft, rectTop}, {dodgeW, dodgeH});
    }

    int col = 0;
    int row = 0;
    int actionRow = 1;

    if (shootingTimer > 0.0f) {
        if (facingUp) actionRow = 3;
        else actionRow = 2;
    }

    if (isJumping) {
        if (shootingTimer > 0.0f) {
            row = actionRow;
            col = 2;
        } else {
            col = 5;
            row = 1 + currentFrame;
        }
    }
    else if (isCrouching) {
        row = 0;
        col = 1;
    }
    else {
        if (isRunning) {
            row = actionRow;
            col = currentFrame;
        }
        else {
            if (shootingTimer > 0.0f) {
                row = actionRow;
                col = 0;
            }
            else {
                if (idleTimer > 2.5f) {
                    row = 0; col = 0;
                } else {
                    row = 1; col = 0;
                }
            }
        }
    }

    animationRow = row;
    return {sf::Vector2i(col * m_frameSize.x, row * m_frameSize.y), m_frameSize};
}

void Player::applySpriteOriginCorrection() {
    constexpr float GLOBAL_FEET_OFFSET = -11.f;

    if (isDodging) {
        const float dodgeWidth = static_cast<float>(m_frameSize.x) / 2.f;
        const float dodgeHeight = static_cast<float>(m_frameSize.y) / 2.f;

        sprite.setOrigin(sf::Vector2f(
            dodgeWidth / 2.f,
            dodgeHeight + GLOBAL_FEET_OFFSET
        ));
    }
    else {
        sprite.setOrigin(sf::Vector2f(
            static_cast<float>(m_frameSize.x) / 2.f,
            static_cast<float>(m_frameSize.y) + GLOBAL_FEET_OFFSET
        ));
    }
}

void Player::updateSpriteDirection() {
    sprite.setScale(sf::Vector2f(facingRight ? 1.f : -1.f, 1.f));
}

void Player::fire(const sf::Vector2f& direction) {
    if (direction.y < 0) facingUp = true;
    else {
        facingUp = false;
        if (direction.x != 0) facingRight = direction.x > 0;
    }
    updateSpriteDirection();

    if (weapon->tryFire(*this, direction)) shootingTimer = shootingDuration;
}

void Player::takeDamage(const int damageAmount) {
    if (isDodging) return;

    health -= damageAmount;
    isHit = true;
    damageEffectTimer = damageEffectDuration;
    damageOverlay.setFillColor(sf::Color(255, 0, 0, 150));
}

void Player::processKill(const int scoreReward) {
    enemiesDefeated++;
    totalScore += scoreReward;

    checkTierUpgrade();
}

void Player::addSkinUnlock(int score, const sf::Texture& texture, const sf::SoundBuffer& sound) {
    m_availableSkins.push_back({score, &texture, &sound, false});
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
    float offsetX = facingUp ? 25.f : static_cast<float>(m_frameSize.x) * 0.25f;
    if (!facingRight) offsetX = facingUp ? -10.f : -static_cast<float>(m_frameSize.x) * 0.3f;

    float offsetY = facingUp ? -(static_cast<float>(m_frameSize.y) * 0.9f)
                             : -(static_cast<float>(m_frameSize.y) * 0.47f);

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

void Player::checkTierUpgrade() {
    for (auto&[scoreThreshold, texture, sound, unlocked] : m_availableSkins) {
        if (!unlocked && totalScore >= scoreThreshold) {
            changeSkin(*texture);

            if (sound) {
                activeSounds.emplace_back(*sound);
                activeSounds.back().play();
            }

            unlocked = true;

            max_health *= 1.2f;
            health += max_health;
            speed *= 1.2f;
            maxJump *= 1.2f;
        }
    }
}

void Player::changeSkin(const sf::Texture& newTexture) {
    sprite.setTexture(newTexture);
    constexpr int cols = 6;
    constexpr int rows = 5;

    m_frameSize.x = static_cast<int>(newTexture.getSize().x) / cols;
    m_frameSize.y = static_cast<int>(newTexture.getSize().y) / rows;

    applySpriteOriginCorrection();
    sprite.setTextureRect(calculateAnimationRect());
}

Player::~Player() {
    std::cout << "Destructor Player\n";
}

void Player::spawn(const float x, const float y) {
    setPosition(x, y);
    velocity = {0.f, 0.f};
    isRunning = false;
    isJumping = false;
    isHit = false;
    damageOverlay.setFillColor(sf::Color::Transparent);
    updateHitbox();
}

void Player::resurrect() {
    health = max_health;
    totalScore = 0;
    enemiesDefeated = 0;
    alive = true;
    isHit = false;
    velocity = {0.f, 0.f};
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