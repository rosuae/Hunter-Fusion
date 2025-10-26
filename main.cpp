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
    float speed;
    sf::Texture texture;
    sf::Sprite sprite;

public:
    Projectile(const std::string& n, int d, sf::Texture& tex, sf::Vector2f playerPos):
    nume{n},
    dmg{d},
    position{playerPos},
    speed{1000.f},
    texture{tex},
    sprite{texture}
    {
        sprite.setOrigin(sf::Vector2f(static_cast<float>(texture.getSize().x), static_cast<float>(texture.getSize().y) / 2.f));
        sprite.scale(sf::Vector2f(0.3f, 0.3f));
        std::cout<<"Constructor proiectil \n";
    }

    friend std::ostream& operator<< (std::ostream& out, const Projectile& p) {
        out << " Nume munitie: " << p.nume << " " << "DMG: "<< p.dmg << "\n";
        return out;
    }

    void projectileTravel (float deltaTime, sf::Vector2f playerPos, sf::RenderWindow& window) {
        sf::Vector2i localPosition = sf::Mouse::getPosition(window);
        sf::Vector2f worldPosition = window.mapPixelToCoords(localPosition);

        float dirX = worldPosition.x - playerPos.x;
        float dirY = worldPosition.y - playerPos.y;

        float length = std::sqrt(dirX * dirX + dirY * dirY);

        if (length != 0) {
            dirX /= length;
            dirY /= length;
        }

        position.x += dirX * speed * deltaTime;
        position.y += dirY * speed * deltaTime;

        sprite.setPosition(sf::Vector2f (position.x, position.y));
    }

    void drawProjectile(sf::RenderWindow& window) {
        window.draw(sprite);
    }
};

class Weapon {
    std::string nume;
    Projectile ammo;
    int ammoamount;
    float firerate;
    int reloada;
public:
    Weapon(const std::string& n, const Projectile& p, int a):
    nume{n},
    ammo{p},
    ammoamount{a},
    firerate{0.1f},
    reloada{30} {
        std::cout<<"Constructor weapon \n";
    }

    friend std::ostream& operator<< (std::ostream& out, const Weapon& w) {
        out << " Numar total munitie: " << w.ammoamount << " Firerate: " << w.firerate << " Nume arma: " << w.nume << " Munitie per reload: " << w.reloada << " " << w.ammo;
        return out;
    }

    friend void reload(Weapon& w) {
        if (w.ammoamount > 0) {
            int needed = 30 - w.reloada;
            int toReload = std::min(needed, w.ammoamount);
            w.reloada += toReload;
            w.ammoamount -= toReload;
            std::cout << "\n Reloaded:  " << toReload << "\n";
            std::cout << "Ammo Amount: " << w.ammoamount << "\n";
        }
        else
            std::cout << "Out of ammo \n";
    }

    friend void use(Weapon& w) {
        std::cout << "Before use: " << w.reloada << "\n";
        if (w.reloada > 0){
            w.reloada -= 1;
            std::cout << "Fire! \n";
        }
        else {
            std::cout << "Reloading...";
            reload(w);
        }
    }
};

class Player {
    std::string name;
    int health;
    float speed;
    float posX, posY, gravity, velocity, maxJump;
    bool isJumping;
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
            std::cout<< "Pressed A\n";
        }

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D)) {
            posX += speed * deltaTime;
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

    void draw(sf::RenderWindow& window) {
        window.draw(sprite);
    }

    sf::Vector2f getPos() {
        return {posX, posY};
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

    void loadEnemy(sf::RenderWindow& window) {
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

    Projectile ammo("PlasmaOrb", 200, projectileTex, player.getPos());
    Weapon PlasmaG("PlasmaGun", ammo, 120);

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
        use(PlasmaG);
        m--;
        std::this_thread::sleep_for(300ms);
    }
        /////////////////////////////////////////////////////////////////////////
        /// Observație: dacă aveți nevoie să citiți date de intrare de la tastatură,
        /// dați exemple de date de intrare folosind fișierul tastatura.txt
        /// Trebuie să aveți în fișierul tastatura.txt suficiente date de intrare
        /// (în formatul impus de voi) astfel încât execuția programului să se încheie.
        /// De asemenea, trebuie să adăugați în acest fișier date de intrare
        /// pentru cât mai multe ramuri de execuție.
        /// Dorim să facem acest lucru pentru a automatiza testarea codului, fără să
        /// mai pierdem timp de fiecare dată să introducem de la zero aceleași date de intrare.
        ///
        /// Pe GitHub Actions (bife), fișierul tastatura.txt este folosit
        /// pentru a simula date introduse de la tastatură.
        /// Bifele verifică dacă programul are erori de compilare, erori de memorie și memory leaks.
        ///
        /// Dacă nu puneți în tastatura.txt suficiente date de intrare, îmi rezerv dreptul să vă
        /// testez codul cu ce date de intrare am chef și să nu pun notă dacă găsesc vreun bug.
        /// Impun această cerință ca să învățați să faceți un demo și să arătați părțile din
        /// program care merg (și să le evitați pe cele care nu merg).
        ///
        /////////////////////////////////////////////////////////////////////////
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
        float cameraSpeed = 5.0f;
        sf::Clock clock;
        bool shouldExit = false;

        while(window.isOpen()) {
            float deltaTime = clock.restart().asSeconds();

            if(sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::Escape)) {
                shouldExit = true;
            }

            if(shouldExit) {
                window.close();
                std::cout << "Fereastra a fost inchisa (shouldExit == true)\n";
                break;
            }

            player.PlayerMovement(deltaTime);
            enemy.enemyMovement(player.getPos(), deltaTime);
            ammo.projectileTravel(deltaTime, player.getPos(), window);

            sf::Vector2f targetPos = player.getPos();
            cameraPos.x += (targetPos.x - cameraPos.x) * cameraSpeed * deltaTime;
            cameraPos.y += (targetPos.y - cameraPos.y - static_cast<float>(height) / 3.f) * cameraSpeed * deltaTime;

            camera.setCenter(cameraPos);
            window.setView(camera);

            window.clear(sf::Color(0,20,20));

            window.draw(bck);
            player.draw(window);
            enemy.loadEnemy(window);
            ammo.drawProjectile(window);

            window.display();
        }

        std::cout << "Programul a terminat executia\n";
        return 0;
    }