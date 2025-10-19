#include <iostream>
#include <array>
#include <chrono>
#include <thread>
#include <vector>

#include <SFML/Graphics.hpp>

//////////////////////////////////////////////////////////////////////
/// This class is used to test that the memory leak checks work as expected even when using a GUI
class SomeClass {
public:
    explicit SomeClass(int) {}
};

SomeClass *getC() {
    return new SomeClass{2};
}
//////////////////////////////////////////////////////////////////////

class Projectile {
    std::string nume;
    int dmg;

public:
    Projectile(const std::string& n, int d): nume{n}, dmg{d}{}
    void travel();
};

class Weapon {
    std::string nume;
    Projectile ammo;
    float firerate;
    int ammoamount, reloada;
public:
    Weapon(const std::string& n, const Projectile& p):
    nume{n},
    ammo{p},
    firerate{0.1},
    ammoamount{120},
    reloada{30}{}

    void use() {
        ammo.travel();
        decreaseAmmo();
        reload();
    }

private:
    void setAmmo(int ammoAmount) { ammoamount = ammoAmount; }
    void decreaseAmmo() { while (ammoamount > 0) ammoamount--; }

public:
    void reload() {
        setAmmo(30);
    }
};

class Player {
    std::string name;
    Weapon currWeapon;
    int health;
    float posX, posY;
public:
    Player(const std::string& n, const Weapon& w):
    name{n},
    currWeapon{w},
    health{100},
    posX{0},
    posY{0}{}
    void isPlaying();
};

class Enemy {
    std::string nume;
    Weapon fists;
    float posX, posY;
    int health, speed;

public:
    Enemy(const std::string& n, const Weapon& f, float posx_, float posy_):
    nume{n},
    fists{f},
    posX{posx_},
    posY(posy_),
    health{100},
    speed{100}{}

    Enemy(const Enemy& other):
    nume{other.nume},
    fists{other.fists},
    posX{other.posX},
    posY{other.posY},
    health{other.health},
    speed{other.speed}{}

    Enemy& operator= (const Enemy& other) {
        nume = other.nume;
        fists = other.fists;
        posX = other.posX;
        posY = other.posY;
        health = other.health;
        speed = other.speed;
        return *this;
    }

    ~Enemy() = default;
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
    MyPlayer{p}{}

    void addEnemy(const Enemy& enemy) {
        enemies.push_back(enemy);
    }
};

int main() {
    ///
    Projectile ammo("ammo", 200);
    Weapon Samus("samus", ammo);
    Player player("Samus", Samus);

    Map map("Map", 100, 100, player);
    Enemy enemy("eyeball", Samus, 0, 0);
    map.addEnemy(enemy);

    std::cout << "Hello, world!\n";
    std::array<int, 100> v{};
    int nr;
    std::cout << "Introduceți nr: ";
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
    std::cin >> nr;
    /////////////////////////////////////////////////////////////////////////
    for(int i = 0; i < nr; ++i) {
        std::cout << "v[" << i << "] = ";
        std::cin >> v[i];
    }
    std::cout << "\n\n";
    std::cout << "Am citit de la tastatură " << nr << " elemente:\n";
    for(int i = 0; i < nr; ++i) {
        std::cout << "- " << v[i] << "\n";
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

    SomeClass *c = getC();
    std::cout << c << "\n";
    delete c;  // comentarea acestui rând ar trebui să ducă la semnalarea unui mem leak

    sf::RenderWindow window;
    ///////////////////////////////////////////////////////////////////////////
    /// NOTE: sync with env variable APP_WINDOW from .github/workflows/cmake.yml:31
    window.create(sf::VideoMode({1920, 1080}), "Hunter Fusion", sf::Style::Default);
    ///////////////////////////////////////////////////////////////////////////
    std::cout << "Fereastra a fost creată\n";
    ///////////////////////////////////////////////////////////////////////////
    /// NOTE: mandatory use one of vsync or FPS limit (not both)            ///
    /// This is needed so we do not burn the GPU                            ///
    window.setVerticalSyncEnabled(true);                                    ///
    /// window.setFramerateLimit(60);                                       ///
    ///////////////////////////////////////////////////////////////////////////

    while(window.isOpen()) {
        bool shouldExit = false;

        while(const std::optional event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) {
                window.close();
                std::cout << "Fereastra a fost închisă\n";
            }
            else if (event->is<sf::Event::Resized>()) {
                std::cout << "New width: " << window.getSize().x << '\n'
                          << "New height: " << window.getSize().y << '\n';
            }
            else if (event->is<sf::Event::KeyPressed>()) {
                const auto* keyPressed = event->getIf<sf::Event::KeyPressed>();
                std::cout << "Received key " << (keyPressed->scancode == sf::Keyboard::Scancode::X ? "X" : "(other)") << "\n";
                if(keyPressed->scancode == sf::Keyboard::Scancode::Escape) {
                    shouldExit = true;
                }
            }
        }
        if(shouldExit) {
            window.close();
            std::cout << "Fereastra a fost închisă (shouldExit == true)\n";
            break;
        }
        using namespace std::chrono_literals;
        std::this_thread::sleep_for(300ms);

        window.clear();
        window.display();
    }

    std::cout << "Programul a terminat execuția\n";
    return 0;
}
