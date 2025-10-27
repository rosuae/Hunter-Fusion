#include <iostream>
#include <chrono>
#include <thread>
#include <vector>
#include <cmath>
#include <SFML/Graphics.hpp>

class Projectile {
    std::string nume;
    int dmg;
    sf::Vector2f position;
    sf::Vector2f direction;
    float speed;
    sf::Sprite sprite;

public:
    Projectile(const std::string& n, int d, const sf::Texture& tex, sf::Vector2f playerPos, sf::Vector2f targetPos):
    nume{n},
    dmg{d},
    position{playerPos},
    speed{1000.f},
    sprite{tex}
    {

        float dirX = targetPos.x - playerPos.x;
        float dirY = targetPos.y - playerPos.y;
        float length = std::sqrt(dirX * dirX + dirY * dirY);

        if (length != 0) {
            direction.x = dirX / length;
            direction.y = dirY / length;
        } else {
            direction = sf::Vector2f(1.f, 0.f);
        }

        sprite.setOrigin(sf::Vector2f(static_cast<float>(tex.getSize().x), static_cast<float>(tex.getSize().y) / 2.f));
        sprite.scale(sf::Vector2f(1.5f, 1.5f));
        std::cout<<"Constructor proiectil \n";
    }

    friend std::ostream& operator<< (std::ostream& out, const Projectile& p) {
        out << " Nume munitie: " << p.nume << " " << "DMG: "<< p.dmg << "\n";
        return out;
    }

    void projectileTravel (float deltaTime) {

        position.x += direction.x * speed * deltaTime;
        position.y += direction.y * speed * deltaTime;

        sprite.setPosition(position);
    }

    void drawProjectile(sf::RenderWindow& window) const{
        window.draw(sprite);
    }

    bool isOutOfBounds(float maxX, float maxY) const {
        return position.x < -100 || position.x > maxX + 100 || position.y < -100 || position.y > maxY + 100;
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
public:
    Weapon(const std::string& n, const std::string& projName, int projDmg, sf::Texture& tex, int a):
    nume{n},
    projectileName{projName},
    projectileDmg{projDmg},
    projectileTex{&tex},
    ammoamount{a},
    firerate{0.1f},
    reloada{30} {
        std::cout<<"Constructor weapon \n";
    }

    Projectile createProjectile(sf::Vector2f playerPos, sf::Vector2f targetPos) {
        return {projectileName, projectileDmg, *projectileTex, playerPos, targetPos};
    }

    friend std::ostream& operator<< (std::ostream& out, const Weapon& w) {
        out << " Numar total munitie: " << w.ammoamount << " Firerate: " << w.firerate << " Nume arma: " << w.nume << " Munitie per reload: " << w.reloada;
        return out;
    }

    void reload() {
        if (ammoamount > 0) {
            int needed = 30 - reloada;
            int toReload = std::min(needed, ammoamount);
            reloada += toReload;
            ammoamount -= toReload;
            std::cout << "\n Reloaded:  " << toReload << "\n";
            std::cout << "Ammo Amount: " << ammoamount << "\n";
        }
        else
            std::cout << "Out of ammo \n";
    }

    void use() {
        std::cout << "Before use: " << reloada << "\n";
        if (reloada > 0){
            reloada -= 1;
            std::cout << "Fire! \n";
        }
        else {
            std::cout << "press r to reload";
        }
    }

    [[nodiscard]]bool canFire() const {
        return reloada > 0;
    }
};

class Player {
    std::string name;
    int health;
    float speed;
    float posX, posY, gravity, velocity, maxJump;
    bool isJumping;
    bool facingRight = true;
    sf::Texture& texture;
    sf::Sprite sprite;

public:
    Player(const std::string& n, sf::Texture& tex):
    name{n},
    health{100},
    speed{900.0f},
    posX{900.0f},
    posY{900.0f},
    gravity{3000.f},
    velocity{0.0f},
    maxJump{-1100.f},
    isJumping{false},
    texture {tex},
    sprite{texture}
    {
        sprite.setPosition(sf::Vector2f(posX, posY));
        sprite.setOrigin(sf::Vector2f(static_cast<float>(texture.getSize().x) / 2.f, static_cast<float>(texture.getSize().y)));
        sprite.scale(sf::Vector2f(.7f, .7f));
        std::cout<<"Constructor Player \n";
    }

    void PlayerMovement(float deltaTime) {


        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W) && !isJumping) {
            velocity = maxJump;
            isJumping = true;
            std::cout<< "Pressed W \n";
        }

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S)) {
            posY += speed * deltaTime;
            std::cout<< "Pressed S\n";
        }

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A)) {
            posX -= speed * deltaTime;
            facingRight = false;
            sprite.setScale(sf::Vector2f(-.7f, .7f));
            std::cout<< "Pressed A\n";
        }

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D)) {
            posX += speed * deltaTime;
            facingRight = true;
            sprite.setScale(sf::Vector2f(.7f, .7f));
            std::cout<< "Pressed D\n";
        }

        velocity += gravity * deltaTime;
        posY += velocity * deltaTime;

        if (posX < 0.0f)
            posX = 1920.0f;

        if (posX > 1920.0f)
                posX = 0.0f;

        if (posY < 0.0f)
            posY = 0.0f;

        if (posY >= 1000.0f) {
            posY = 1000.f;
            velocity = 0.f;
            isJumping = false;
        }
        sprite.setPosition(sf::Vector2f(posX, posY));
    }

    void draw(sf::RenderWindow& window) const{
        window.draw(sprite);
    }

    sf::Vector2f getPos() const{
        return {posX, posY};
    }

    sf::Vector2f getWeaponTipPos() const {
        float offsetX = 30.f;
        float offsetY = -180.f;

        if (!facingRight) {
            offsetX = -offsetX;
        }

        return {posX + offsetX, posY + offsetY};
    }

    friend std::ostream& operator<< (std::ostream& out, const Player& p) {
        out << " Nume player: " << p.name << " Viata: " << p.health<< " Pos X: " << p.posX << " Pos Y: " << p.posY << "Player speed: " << p.speed;
        return out;
    }
};

class Enemy {
    std::string nume;
    Weapon fists;
    float posX, posY;
    int health;
    float gravity, speed;
    bool alive;
    sf::Texture texture;
    sf::Sprite sprite;

public:
    Enemy(const std::string& n, const Weapon& f, float posx_, float posy_, sf::Texture& tex):
    nume{n},
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

    ~Enemy() = default;

    void enemyMovement(sf::Vector2f playerpos, float deltaTime) {

        if (posX >= playerpos.x) {
            posX -= speed * deltaTime;
        }
        if (posX <= playerpos.x) {
            posX += speed * deltaTime;
        }

        posY += gravity * deltaTime;

        if (posY >= 1000.f)
            posY = 1000.f;

        sprite.setPosition(sf::Vector2f(posX, posY));
    }

    void loadEnemy(sf::RenderWindow& window) const{
            window.draw(sprite);
    }

    friend std::ostream& operator<< (std::ostream& out, const Enemy& e) {
        out << " Nume inamic: " << e.nume << " Pos X: " << e.posX << " Pos Y: " << e.posY << " Viata inamic: " << e.health << " Viteza imanic: " << e.speed << " " << e.fists;
        return out;
    }
};

class Map {
    std::string MapNume;
    int sizeX, sizeY;
    Player MyPlayer;
    std::vector<Enemy> enemies;

public:
    Map(const std::string& n, int sizex_, int sizey_, const Player& p):
    MapNume{n},
    sizeX{sizex_},
    sizeY{sizey_},
    MyPlayer{p} {
        std::cout<<"Constructor Map\n";
    }

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
};

int main() {

    sf::Texture projectileTex;
    if (projectileTex.loadFromFile("assets/textures/projectile.png"))
        std::cout << "Eroare la incarcarea texturei pentru Projectile";

    sf::Texture texture;
        if (!texture.loadFromFile("assets/textures/samustest.png"))
            std::cout << "Eroare la deschidere fisier \n";

    Player player("Samus", texture);

    Weapon PlasmaG("PlasmaGun", "PlasmaOrb", 100, projectileTex,120);

    sf::Texture backround;
    if (!backround.loadFromFile("assets/textures/map/background.png")){
        std::cout << "Eroare la deschidere fisier background";
    }

    sf::Sprite bck(backround);
    bck.setPosition(sf::Vector2f(0.f, 0.f));
    bck.scale(sf::Vector2f(1.66f, 2.f));


    Map map("Map", 1000, 1000, player);

    sf::Texture textureE;
    if (!textureE.loadFromFile("assets/textures/samustleft.png"))
        std::cout << "Eroare la deschidere fisier \n";

    Enemy enemy("Metroid", PlasmaG, 1500.0f, 200.0f, textureE);
    map.addEnemy(enemy);

    using namespace std::chrono_literals;
    int m = 1;
    while (m) {
        PlasmaG.use();
        m--;
        std::this_thread::sleep_for(300ms);
    }
        ///////////////////////////////////////////////////////////////////////////
        /// Pentru date citite din fișier, NU folosiți tastatura.txt. Creați-vă voi
        /// alt fișier propriu cu ce alt nume doriți.
        /// Exemplu:
        /// std::ifstream fis("date.txt");
        /// for(int i = 0; i < nr2; ++i)
        ///     fis >> v2[i];
        ///
        ///////////////////////////////////////////////////////////////////////////

        sf::RenderWindow window;
        const sf::VideoMode desktop = sf::VideoMode::getDesktopMode();
        const unsigned int width = desktop.size.x;
        const unsigned int height = desktop.size.y;

        window.create(sf::VideoMode({width, height}, desktop.bitsPerPixel), "Hunter Fusion", sf::Style::Default, sf::State::Fullscreen);
        ///////////////////////////////////////////////////////////////////////////
        /// NOTE: sync with env variable APP_WINDOW from .github/workflows/cmake.yml:31
        // window.create(sf::VideoMode({1920, 1080}), "Hunter Fusion", sf::Style::Default);
        ///////////////////////////////////////////////////////////////////////////
        std::cout << "Fereastra a fost creată\n";
        ///////////////////////////////////////////////////////////////////////////
        /// NOTE: mandatory use one of vsync or FPS limit (not both)            ///
        /// This is needed so we do not burn the GPU                            ///
        window.setVerticalSyncEnabled(true);                                    ///
        /// window.setFramerateLimit(60);                                       ///
        ///////////////////////////////////////////////////////////////////////////
        sf::View camera(sf::FloatRect(sf::Vector2f(0.f, 0.f), sf::Vector2f(static_cast<float>(width), static_cast<float>(height))));
        window.setView(camera);

        sf::Vector2f cameraPos = player.getPos();

        sf::Clock clock;
        std::vector<Projectile> projectiles;

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
                }

                if (const auto* mousePress = event->getIf<sf::Event::MouseButtonPressed>()) {
                    if (mousePress->button == sf::Mouse::Button::Left && PlasmaG.canFire()) {
                        sf::Vector2i mousePixel = sf::Mouse::getPosition(window);
                        sf::Vector2f mouseWorld = window.mapPixelToCoords(mousePixel);

                        projectiles.push_back(PlasmaG.createProjectile(player.getWeaponTipPos(), mouseWorld));
                        PlasmaG.use();
                    }
                }

                if (const auto* keyPress = event->getIf<sf::Event::KeyPressed>()) {
                    if (keyPress->scancode == sf::Keyboard::Scancode::R) {
                        PlasmaG.reload();
                    }
                }
            }

            if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left)) {
                std::cout << "Click stanga" << std::endl;
            }
            else if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Right)) {
                std::cout << "Click dreapta" << std::endl;
            }

            player.PlayerMovement(deltaTime);
            enemy.enemyMovement(player.getPos(), deltaTime);

            for (auto& proj : projectiles) {
                proj.projectileTravel(deltaTime);
            }

            std::erase_if(projectiles, [](const Projectile& p) {
                return p.isOutOfBounds(5000.f, 5000.f);
            });

            sf::Vector2f targetPos = player.getPos();
            cameraPos.x += (targetPos.x - cameraPos.x) * cameraSpeed * deltaTime;
            cameraPos.y += (targetPos.y - cameraPos.y - static_cast<float>(height) / 3.f) * cameraSpeed * deltaTime;

            camera.setCenter(cameraPos);
            window.setView(camera);

            window.clear(sf::Color(0,20,20));

            window.draw(bck);
            enemy.loadEnemy(window);

            for (auto& proj : projectiles) {
                proj.drawProjectile(window);
            }
            player.draw(window);

            window.display();
        }

        std::cout << "Programul a terminat executia\n";
        return 0;
    }