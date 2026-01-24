#ifndef OOP_ENEMY_H
#define OOP_ENEMY_H

#include "Weapon.h"
#include "Entity.h"
#include "EnemyFactory.h"
#include "EnemyStrategies.h"

class Map;
class Pickup;
class Player;

class Enemy : public Entity{
    int damage;
    int bountyScore;
    static int activeEnemyCount;
    Entity* target;
    bool isMoving;
    
    std::unique_ptr<EnemyStrategy> strategy;

    float detectionRange;
    
    sf::Vector2i frameSize;
    int currentFrame;
    float animationTimer;
    float frameDuration;
    int animationFrameCount;

    const sf::Texture* alertTexture;
    sf::Sprite exclamationSprite;
    sf::Vector2i alertFrameSize;

    float alertAnimTimer;
    bool alertActive;

    std::list<sf::Sound>* activeSounds;
    const sf::SoundBuffer* hitSound;
    const sf::SoundBuffer* deathSound;

    sf::FloatRect doGetBounds() const override;
    void applyGravity(float deltaTime) override;

    void takeDamage(int damageAmount) override;
    void doBehavior(float deltaTime, const Map& map) override;
    
    void updateAI(float deltaTime);
    void updatePhysics(float deltaTime, const Map& map);
    void updateAnimation(float deltaTime);
    void draw(sf::RenderWindow& window) const override;

    friend class EnemyFactory;

    Enemy(const std::string& n,
        int damage_, int bountyScore_,
        float posx_, float posy_,
        sf::Texture& tex, const sf::Texture& alertTex,
        std::list<sf::Sound>& activeSounds_,
        const sf::SoundBuffer& hitSound_,
        const sf::SoundBuffer& deathSound_,
        Entity* target_);

public:
    friend std::ostream& operator<< (std::ostream& out, const Enemy& e) {
        out << " Nume inamic: " << e.name << " Pos X: " << e.posX << " Pos Y: " << e.posY << " Viata inamic: " << e.health << " Viteza imanic: " << e.speed;
        return out;
    }

    ~Enemy() override{
            activeEnemyCount--;
        }

    Enemy(const Enemy &other);

    friend void swap(Enemy &lhs, Enemy &rhs) noexcept;

    Enemy & operator=(Enemy other) {
        Entity::operator=(other);
        swap(*this, other);
        return *this;
    }

    std::unique_ptr<Entity> clone() const override{
        return std::make_unique<Enemy>(*this);
    }

    static int getActiveEnemyCount();
    void grantReward(Player& player) const;
    void onDeath(Map &map) override;
    // void onCollision(Player &player) override;
    int getCollisionPriority() const override { return PRIORITY_HAZARD; }

    void setStrategy(std::unique_ptr<EnemyStrategy> newStrategy);
    
    float getDistanceToTarget() const;
    bool isTouchingTarget() const;
    float getDetectionRange() const { return detectionRange; }
    sf::Vector2f getTargetPos() const;
    float getSpeed() const { return speed; }
    int getDamage() const { return damage; }

    void setMoving(const bool moving) { isMoving = moving; }
    void setX(const float x) { posX = x; }
    void setScale(float x, float y) { sprite.setScale({x, y}); }
    void tryDealDamageToTarget(int dmgAmount) const;
};

#endif
