#include <iostream>
#include <chrono>
#include <thread>
#include <vector>
#include <cmath>
#include <random>
#include <fstream>
#include <list>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>

class Projectile {
    std::string nume;
    int dmg;
    sf::Vector2f position;
    sf::Vector2f direction;
    float speed;
    sf::Sprite sprite;
    bool active;

    void calculateDirection(sf::Vector2f playerPos, sf::Vector2f targetPos) {
        float dirX = targetPos.x - playerPos.x;
        float dirY = targetPos.y - playerPos.y;
        float length = std::sqrt(dirX * dirX + dirY * dirY);

        if (length != 0) {
            direction.x = dirX / length;
            direction.y = dirY / length;
        } else {
            direction = sf::Vector2f(1.f, 0.f);
        }
    }

    void setupSprite(const sf::Texture& tex) {
        sprite.setOrigin(sf::Vector2f(static_cast<float>(tex.getSize().x),
                                      static_cast<float>(tex.getSize().y) / 2.f));
        sprite.scale(sf::Vector2f(1.5f, 1.5f));
    }

    void updatePosition(float deltaTime) {
        position.x += direction.x * speed * deltaTime;
        position.y += direction.y * speed * deltaTime;
        sprite.setPosition(position);
    }

public:
    Projectile(std::string n, int d, const sf::Texture& tex, sf::Vector2f playerPos, sf::Vector2f targetPos):
    nume{std::move(n)},
    dmg{d},
    position{playerPos},
    speed{1000.f},
    sprite{tex},
    active{true}
    {
        calculateDirection(playerPos, targetPos);
        setupSprite(tex);
        std::cout<<"Constructor proiectil \n";
    }

    ~Projectile() { std::cout << "S a apelat destructor projectile \n";}

    friend std::ostream& operator<< (std::ostream& out, const Projectile& p) {
        out << " Nume munitie: " << p.nume << " " << "DMG: "<< p.dmg << "\n";
        return out;
    }

    void projectileTravel (float deltaTime) {
        updatePosition(deltaTime);
    }

    void deactivate() {
        active = false;
    }

    bool isActive() const {
        return active;
    }

    void drawProjectile(sf::RenderWindow& window) const{
        window.draw(sprite);
    }

    sf::FloatRect getBounds() const {
        return sprite.getGlobalBounds();
    }

    int getDamage() const {
        return dmg;
    }

    bool isOutOfBounds(float maxX, float maxY) const {
        return position.x < -100 || position.x > maxX + 100 || position.y < -100 || position.y > maxY + 100;
    }

    bool shouldBeRemoved(float maxX, float maxY) const {
        return !active || isOutOfBounds(maxX, maxY);
    }
};

class Weapon {
    std::string nume;
    std::string projectileName;
    int projectileDmg;
    sf::Texture* projectileTex;
    int ammoamount;
    float firerate;
    int reloada;
    std::vector<Projectile> projectiles;

    [[nodiscard]]int calculateReloadAmount() const {
        int needed = 30 - reloada;
        return std::min(needed, ammoamount);
    }

    [[nodiscard]]bool hasAmmoToReload() const {
        return ammoamount > 0 && reloada < 30;
    }

    void transferAmmo(int amount) {
        reloada += amount;
        ammoamount -= amount;
    }

public:
    Weapon(std::string n, std::string projName, int projDmg, sf::Texture& tex, int a):
    nume{std::move (n)},
    projectileName{std::move(projName)},
    projectileDmg{projDmg},
    projectileTex{&tex},
    ammoamount{a},
    firerate{0.1f},
    reloada{30} {
        std::cout<<"Constructor weapon \n";
    }

    ~Weapon() { std::cout << "S a apelat destructor Weapon \n";}

    friend std::ostream& operator<< (std::ostream& out, const Weapon& w) {
        out << " Numar total munitie: " << w.ammoamount << " Firerate: " << w.firerate << " Nume arma: " << w.nume << " Munitie per reload: " << w.reloada;
        return out;
    }

    void reload(std::list<sf::Sound>& sounds, const sf::SoundBuffer& buffer) {
        if (hasAmmoToReload()) {
            int toReload = calculateReloadAmount();
            transferAmmo(toReload);

            sounds.emplace_back(buffer);
            sounds.back().play();

            std::cout << "\n Reloaded:  " << toReload << "\n";
            std::cout << "Ammo Amount: " << ammoamount << "\n";
        }
        else
            std::cout << "Out of ammo \n";
    }

    [[nodiscard]]int getDmg() const{
        return projectileDmg;
    }

    [[nodiscard]]bool canFire() const {
        return reloada > 0;
    }

    void updateProjectiles(float deltaTime) {
        for (auto& proj : projectiles) {
            if (proj.isActive()) {
                proj.projectileTravel(deltaTime);
            }
        }

        std::erase_if(projectiles, [](const Projectile& p) {
            return p.shouldBeRemoved(1920.f, 1080.f);
        });
    }

    std::vector<Projectile>& getProjectiles() {
        return projectiles;
    }

    void fire(sf::Vector2f playerPos, sf::Vector2f targetPos, std::list<sf::Sound>& sounds, const sf::SoundBuffer& buffer) {
        if (canFire()) {
            projectiles.emplace_back(projectileName, projectileDmg, *projectileTex, playerPos, targetPos);
            reloada -= 1;

            sounds.emplace_back(buffer);
            sounds.back().play();

            std::cout << "Fire! \n";
        } else {
            std::cout << "press r to reload\n";
        }
    }

    void drawProjectiles(sf::RenderWindow& window) const {
        for (const auto& proj : projectiles) {
            proj.drawProjectile(window);
        }
    }
};

class Player {
    std::string name;
    int health;
    float speed;
    float posX, posY, gravity, velocity, maxJump;
    bool isJumping, isalive = true, isHit_ = false, facingRight = true;

    sf::Texture& texture;
    sf::Sprite sprite;
    sf::RectangleShape damageOverlay;

    void applyGravity(float deltaTime) {
        velocity += gravity * deltaTime;
        posY += velocity * deltaTime;
    }

    void handleGroundCollision() {
        if (posY >= 1000.0f) {
            posY = 1000.f;
            velocity = 0.f;
            isJumping = false;
        }
    }

    void handleScreenBarriers() {
        if (posX < 0.0f)
            posX = 1920.0f;
        if (posX > 1920.0f)
            posX = 0.0f;
        if (posY < 0.0f)
            posY = 0.0f;
    }

    float calculateWeaponOffsetX() const {
        return facingRight ? 30.f : -30.f;
    }

    void updateSpriteDirection() {
        if (facingRight)
            sprite.setScale(sf::Vector2f(.5f, .5f));
        else
            sprite.setScale(sf::Vector2f(-.5f, .5f));
    }

    void checkDeath() {
        if (health <= 0) {
            health = 0;
            isalive = false;
            std::cout << "GAME OVER \n";
        }
    }

public:
    Player(std::string n, sf::Texture& tex):
    name{std::move(n)},
    health{100},
    speed{900.0f},
    posX{900.0f},
    posY{900.0f},
    gravity{2000.f},
    velocity{0.0f},
    maxJump{-1000.f},
    isJumping{false},
    texture {tex},
    sprite{texture}
    {   damageOverlay.setSize(sf::Vector2f(1920.f, 1080.f));
        damageOverlay.setFillColor(sf::Color(255, 0, 0, 0));

        sprite.setPosition(sf::Vector2f(posX, posY));
        sprite.setOrigin(sf::Vector2f(static_cast<float>(texture.getSize().x) / 2.f, static_cast<float>(texture.getSize().y)));
        sprite.scale(sf::Vector2f(.5f, .5f));
        std::cout<<"Constructor Player \n";
    }

    ~Player() { std::cout << "S a apelat destructor Player \n";}

    void PlayerMovement(float deltaTime, std::list<sf::Sound>& sounds, const sf::SoundBuffer& buffer) {

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

    void draw(sf::RenderWindow& window) const{
        sf::RectangleShape healthbar(sf::Vector2f(100.f, 10.f));

        healthbar.setPosition(sf::Vector2f(posX - 50.f, posY - 250.f));
        healthbar.setFillColor(sf::Color::Green);
        healthbar.setSize(sf::Vector2f(static_cast<float>(health), 10.f));

        window.draw(healthbar);
        window.draw(sprite);
    }

    sf::Vector2f getPos() const{
        return {posX, posY};
    }

    sf::Vector2f getWeaponTipPos() const {
        float offsetX = calculateWeaponOffsetX();
        float offsetY = -180.f;

        return {posX + offsetX, posY + offsetY};
    }

    void takeDamage (int damageAmount) {
        health -= damageAmount;
        checkDeath();
        if (isalive) {
            std::cout << "Player ul a primit " << damageAmount << " dmg, ramanand cu " << health << " viata \n";
        }
    }

    bool isHit () const{
        return isHit_;
    }

    void setHit (const bool ok) {
        isHit_ = ok;
    }

    void resetDamageEffect() {
        damageOverlay.setFillColor(sf::Color(255, 0, 0, 0));
    }

    void alphaDamageEffect(int alpha) {
        damageOverlay.setFillColor(sf::Color(255, 0, 0, alpha));
    }

    void drawDamageEffect(sf::RenderWindow& window) const{
        const sf::View currentView = window.getView();

        window.setView(window.getDefaultView());
        window.draw(damageOverlay);
        window.setView(currentView);
    }

    sf::FloatRect getBounds () const {
        return sprite.getGlobalBounds();
    }

    bool isAlive() const {
        return isalive;
    }

    friend std::ostream& operator<< (std::ostream& out, const Player& p) {
        out << " Nume player: " << p.name << " Viata: " << p.health<< " Pos X: " << p.posX << " Pos Y: " << p.posY << "Player speed: " << p.speed;
        return out;
    }
};

class Enemy {
    std::string nume;
    Weapon* fists;
    float posX, posY;
    int health;
    float gravity, speed;
    bool alive;
    sf::Texture& texture;
    sf::Sprite sprite;

    void moveTowardsPlayer(sf::Vector2f playerPos, float deltaTime) {
        if (posX >= playerPos.x) {
            posX -= speed * deltaTime;
        }
        if (posX <= playerPos.x) {
            posX += speed * deltaTime;
        }
    }

    void applyGravity(float deltaTime) {
        posY += gravity * deltaTime;
        if (posY >= 1000.f)
            posY = 1000.f;
    }

    void updateSpritePosition() {
        sprite.setPosition(sf::Vector2f(posX, posY));
    }

    void checkDeath(std::list<sf::Sound>& sounds, const sf::SoundBuffer& buffer) {
        if (health <= 0) {
            health = 0;

            sounds.emplace_back(buffer);
            sounds.back().play();

            alive = false;
            std::cout << "inamic invins \n";
        }
    }

public:
    Enemy(std::string n, Weapon* f, float posx_, float posy_, sf::Texture& tex):
    nume{std::move(n)},
    fists{f},
    posX{posx_},
    posY(posy_),
    health{100},
    gravity{3000.f},
    speed{300.f},
    alive{true},
    texture{tex},
    sprite{texture} {
        sprite.setPosition(sf::Vector2f(posX, posY));
        sprite.setOrigin(sf::Vector2f(static_cast<float>(texture.getSize().x) / 2.f, static_cast<float>(texture.getSize().y)));
        sprite.setScale(sf::Vector2f(.7f, .7f));
        std::cout<<"Constructor Enemy \n";
    }

    Enemy (const Enemy& other):
    nume{other.nume},
    fists{other.fists},
    posX{other.posX},
    posY{other.posY},
    health{other.health},
    gravity{other.gravity},
    speed{other.speed},
    alive{other.alive},
    texture{other.texture},
    sprite{other.sprite}
    {
        std::cout<<"Constructor de copiere\n";
    }

    Enemy& operator= (const Enemy& other) {
        if (this == &other) {
            return *this;
        }

        nume = other.nume;
        fists = other.fists;
        posX = other.posX;
        posY = other.posY;
        health = other.health;
        speed = other.speed;
        gravity = other.gravity;
        alive = other.alive;
        texture = other.texture;
        sprite = other.sprite;
        std::cout<<"S-a folosit supraincarcarea op= pentru clasa Enemy \n";
        return *this;
    }

    ~Enemy() {std::cout << "S a apelat destructor Enemy \n";}

    void enemyMovement(sf::Vector2f playerpos, float deltaTime) {
        moveTowardsPlayer(playerpos, deltaTime);
        applyGravity(deltaTime);
        updateSpritePosition();
    }

    void takeDamage(int damageAmount, std::list<sf::Sound>& sounds, const sf::SoundBuffer& buffer, const sf::SoundBuffer& buffer1) {
        health -= damageAmount;

        sounds.emplace_back(buffer);
        sounds.back().play();

        checkDeath(sounds, buffer1);
        if (alive) {
            std::cout << "a primit " << damageAmount << " dmg, mai are " << health << "\n";
        }
    }

    void loadEnemy(sf::RenderWindow& window) const{
            window.draw(sprite);
    }

    sf::FloatRect getBounds() const{
        return sprite.getGlobalBounds();
    }

    int getContactDamage() const {
        return fists->getDmg();
    }

    bool isAlive() const{
        return alive;
    }

    friend std::ostream& operator<< (std::ostream& out, const Enemy& e) {
        out << " Nume inamic: " << e.nume << " Pos X: " << e.posX << " Pos Y: " << e.posY << " Viata inamic: " << e.health << " Viteza imanic: " << e.speed << " " << *(e.fists);
        return out;
    }
};

class Map {
    std::string MapNume;
    int sizeX, sizeY;
    Player& MyPlayer;
    std::vector<Enemy> enemies;

public:

    Map(std::string n, int sizex_, int sizey_, Player& p):
    MapNume{std::move(n)},
    sizeX{sizex_},
    sizeY{sizey_},
    MyPlayer{p} {
        std::cout<<"Constructor Map\n";
    }

    ~Map() { std::cout << "S a apelat destructor Map \n";}

    friend std::ostream& operator<< (std::ostream& out, const Map& m) {
        out << " Nume harta: " << m.MapNume << " Latime harta: " << m.sizeX << " Lungime harta: " << m.sizeY << " " << m.MyPlayer;
        out << "\n Enemies: \n";
        for (const auto& e : m.enemies) {
            out << e << "\n";
        }
        return out;
    }

    void addEnemy(const Enemy& enemy) {
        enemies.push_back(enemy);
    }

    std::vector<Enemy>& getEnemies() {
        return enemies;
    }
};

bool intersects(const sf::FloatRect& rect1, const sf::FloatRect& rect2) {
    return rect1.findIntersection(rect2).has_value();
}

int randomInt(int min, int max) {
    static std::mt19937 gen(std::random_device{}());
    return std::uniform_int_distribution(min, max)(gen);
}

int main() {

    std::ifstream fin("date.txt");

    if (!fin.is_open()) {
        std::cout << "Eroare la deschiderea fisierului date.txt\n";
        return 1;
    }

    std::string projectileTexPath, playerTexPath, backgroundPath, enemyTexPath;
    fin >> projectileTexPath >> playerTexPath >> backgroundPath >> enemyTexPath;

    std::string enemyDmgSPath, enemyDeathSPath, jumpSPath, reloadSPath, shootSPath;
    fin >> enemyDmgSPath >> enemyDeathSPath >> jumpSPath >> reloadSPath >> shootSPath;

    sf::Texture projectileTex;
    if (!projectileTex.loadFromFile(projectileTexPath))
        std::cout << "Eroare la incarcarea texturii pentru Projectile: " << projectileTexPath << "\n";

    sf::Texture playerTex;
    if (!playerTex.loadFromFile(playerTexPath))
        std::cout << "Eroare la deschidere textura: " << playerTexPath << "\n";

    sf::Texture backround;
    if (!backround.loadFromFile(backgroundPath))
        std::cout << "Eroare la deschidere textura background: " << backgroundPath << "\n";

    sf::Texture enemyTex;
    if (!enemyTex.loadFromFile(enemyTexPath))
        std::cout << "Eroare la deschidere textura: " << enemyTexPath << "\n";

    sf::Sprite bck(backround);
    bck.setPosition(sf::Vector2f(-100.f, -200.f));
    bck.scale(sf::Vector2f(8.f, 8.f));

    std::string playerName, playerWeapon, projectileName, mapName, enemyWeapon, enemyProj;
    fin >> playerName >> playerWeapon >> projectileName >> mapName >> enemyWeapon >> enemyProj;

    Player player(playerName, playerTex);
    Weapon PlasmaG(playerWeapon, projectileName, 25, projectileTex, 120);
    Map map(mapName, 1000, 1000, player);
    Weapon fists(enemyWeapon, enemyProj, 10, projectileTex, 1000);

    sf::SoundBuffer shootBuffer;
    if (!shootBuffer.loadFromFile(shootSPath)) {
        std::cout << "Eroare la incarcarea sunetului\n";
    }

    sf::SoundBuffer jumpBuffer;
    if (!jumpBuffer.loadFromFile(jumpSPath)) {
        std::cout << "Eroare la incarcarea sunetului\n";
    }

    sf::SoundBuffer reloadBuffer;
    if (!reloadBuffer.loadFromFile(reloadSPath)) {
        std::cout << "Eroare la incarcarea sunetului\n";
    }

    sf::SoundBuffer enemyDeathBuffer;
    if (!enemyDeathBuffer.loadFromFile(enemyDeathSPath)) {
        std::cout << "Eroare la incarcarea sunetului\n";
    }

    sf::SoundBuffer enemyDamageBuffer;
    if (!enemyDamageBuffer.loadFromFile(enemyDmgSPath)) {
        std::cout << "Eroare la incarcarea sunetului\n";
    }

    std::vector<std::string> enemyNames;
    std::string enemyName;
    while (fin >> enemyName) {
        enemyNames.push_back(enemyName);
    }

    fin.close();

    for (const auto& name : enemyNames) {
        map.addEnemy({name, &fists,
                      static_cast<float>(randomInt(100, 1920)),
                      static_cast<float>(randomInt(100, 400)),
                      enemyTex});
    }

        sf::RenderWindow window;
        const sf::VideoMode desktop = sf::VideoMode::getDesktopMode();
        const unsigned int width = desktop.size.x;
        const unsigned int height = desktop.size.y;

        window.create(sf::VideoMode({width, height}, desktop.bitsPerPixel), "Hunter Fusion", sf::Style::Default, sf::State::Fullscreen);
        ///////////////////////////////////////////////////////////////////////////
        /// NOTE: sync with env variable APP_WINDOW from .github/workflows/cmake.yml:31
        ///////////////////////////////////////////////////////////////////////////
        std::cout << "Fereastra a fost creată\n";
        window.setVerticalSyncEnabled(true);

        sf::View camera(sf::FloatRect(sf::Vector2f(0.f, 0.f), sf::Vector2f(static_cast<float>(width), static_cast<float>(height))));
        window.setView(camera);
        sf::Vector2f cameraPos = player.getPos();

    std::list<sf::Sound> playingSounds;

    sf::Clock clock;
    sf::Clock playerDamageCooldown;
    sf::Clock damageClock;

    while(window.isOpen()) {
        float deltaTime = clock.restart().asSeconds();
        float cameraSpeed = 5.0f;

        while (const std::optional<sf::Event> event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) {
                window.close();
            }

            if (const auto* keyPress = event->getIf<sf::Event::KeyPressed>()) {
                if (keyPress->scancode == sf::Keyboard::Scancode::Escape) {
                    window.close();
                }

                if (keyPress->scancode == sf::Keyboard::Scancode::R) {
                    PlasmaG.reload(playingSounds, reloadBuffer);
                }
            }

            if (const auto* mousePress = event->getIf<sf::Event::MouseButtonPressed>()) {
                if (mousePress->button == sf::Mouse::Button::Left && PlasmaG.canFire() && player.isAlive()) {
                    sf::Vector2i mousePixel = sf::Mouse::getPosition(window);
                    sf::Vector2f mouseWorld = window.mapPixelToCoords(mousePixel);

                    PlasmaG.fire(player.getWeaponTipPos(), mouseWorld, playingSounds, shootBuffer);
                }
            }
        }

        if (player.isAlive()) {
            player.PlayerMovement(deltaTime, playingSounds, jumpBuffer);
        }

        for (auto& en : map.getEnemies()) {
            if (en.isAlive()) {
                en.enemyMovement(player.getPos(), deltaTime);
            }
        }

        PlasmaG.updateProjectiles(deltaTime);

        for (auto& proj : PlasmaG.getProjectiles()) {
            if (!proj.isActive()) continue;

            for (auto& en : map.getEnemies()) {
                if (en.isAlive() && intersects(proj.getBounds(), en.getBounds())) {
                    en.takeDamage(proj.getDamage(), playingSounds, enemyDamageBuffer, enemyDeathBuffer);
                    proj.deactivate();
                    break;
                }
            }
        }

        int totalDamageThisFrame = 0;
        for (const auto& en : map.getEnemies()) {
            if (en.isAlive() && player.isAlive() && intersects(player.getBounds(), en.getBounds())) {
                totalDamageThisFrame += en.getContactDamage();
            }
        }

        if (totalDamageThisFrame > 0 && playerDamageCooldown.getElapsedTime().asSeconds() > 1.f) {
            player.takeDamage(totalDamageThisFrame);
            player.setHit(true);
            damageClock.restart();
            playerDamageCooldown.restart();
        }

        if (player.isHit()) {
            float elapsed = damageClock.getElapsedTime().asSeconds();

            if (elapsed < 0.3f) {
                int alpha = static_cast<int>(120 * (1.f - elapsed / 0.3f));
                player.alphaDamageEffect(alpha);
            }
            else {
                player.setHit(false);
                player.resetDamageEffect();
            }
        }

        int enemiesToSpawn = 0;
        for (const auto& en : map.getEnemies()) {
            if (!en.isAlive()) {
                enemiesToSpawn += 2;
            }
        }

        std::erase_if(map.getEnemies(), [](const Enemy& en) {
            return !en.isAlive();
        });

        for (int i = 0; i < enemiesToSpawn; ++i) {
                map.addEnemy({"Metroid", &fists,
                    static_cast<float> (randomInt(100, 1920)),
                    static_cast<float> (randomInt(100, 400)),
                    enemyTex});
        }

        sf::Vector2f targetPos = player.getPos();
        cameraPos.x += (targetPos.x - cameraPos.x) * cameraSpeed * deltaTime;
        cameraPos.y += (targetPos.y - cameraPos.y - static_cast<float>(height) / 3.f) * cameraSpeed * deltaTime;
        camera.setCenter(cameraPos);
        window.setView(camera);

        window.clear(sf::Color::Black);
        window.draw(bck);

        PlasmaG.drawProjectiles(window);

        if (player.isAlive()) {
            player.draw(window);
        }

        for (const auto& en : map.getEnemies()) {
                en.loadEnemy(window);
        }

        player.drawDamageEffect(window);

        playingSounds.remove_if([](const sf::Sound& Sound_) {
        return Sound_.getStatus() == sf::Sound::Status::Stopped;
        });

        window.display();
    }
    return 0;
}