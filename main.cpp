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

#include "src/Map.h"

bool intersects(const sf::FloatRect& rect1, const sf::FloatRect& rect2) {
    return rect1.findIntersection(rect2).has_value();
}

int randomInt(const int min, const int max) {
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

    sf::Texture backround;
    if (!backround.loadFromFile(backgroundPath))
        std::cout << "Eroare la deschidere textura background: " << backgroundPath << "\n";

    sf::Sprite bck(backround);
    bck.setPosition(sf::Vector2f(-100.f, -200.f));
    bck.scale(sf::Vector2f(8.f, 8.f));

    std::string playerName, playerWeapon, projectileName, mapName, enemyWeapon, enemyProj;
    fin >> playerName >> playerWeapon >> projectileName >> mapName >> enemyWeapon >> enemyProj;

    Player player(playerName, playerTexPath);
    Weapon PlasmaG(playerWeapon, projectileName, 25, projectileTexPath, 120);
    Map map(mapName, 1000, 1000, player);
    Weapon fists(enemyWeapon, enemyProj, 10, projectileTexPath, 1000);

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
                      enemyTexPath});
    }

        sf::RenderWindow window;
        const sf::VideoMode desktop = sf::VideoMode::getDesktopMode();
        const unsigned int width = desktop.size.x;
        const unsigned int height = desktop.size.y;

        window.create(sf::VideoMode({width, height}, desktop.bitsPerPixel), "Hunter Fusion", sf::Style::Default, sf::State::Fullscreen);
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
                    enemyTexPath});
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