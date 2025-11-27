#include "settingsMenuState.h"
#include <SFML/Graphics.hpp>

settingsMenuState::settingsMenuState() :
    isFullscreen{false},
    width{1920},
    height{1080},
    shouldStartGame{false},
    texture{"assets/textures/settingsmenu.png"},
    SmenuSprite{texture} {
    launcher.create(sf::VideoMode({800, 600}), "Hunter Fusion", sf::Style::Titlebar | sf::Style::Close);
    launcher.setFramerateLimit(90);
}

void settingsMenuState::SetSettingsLauncher() {

    sf::Font font;
    if (!font.openFromFile("assets/textures/Metroid-Fusion.ttf")) {
    }

    sf::Text textFull(font,"Fullscreen", 40);
    textFull.setPosition({260.f, 160.f});

    sf::Text textWin(font, "Windowed", 40);
    textWin.setPosition({270.f, 300.f});

    while (launcher.isOpen()) {
        while (const std::optional<sf::Event> event = launcher.pollEvent()) {
            if (event->is<sf::Event::Closed>()) {
                launcher.close();
            }

            if (const auto* keyPress = event->getIf<sf::Event::KeyPressed>()) {
                if (keyPress->scancode == sf::Keyboard::Scancode::Escape) {
                    launcher.close();
                }
            }

            if (const auto* mousePress = event->getIf<sf::Event::MouseButtonPressed>()) {
                if (mousePress->button == sf::Mouse::Button::Left) {
                    sf::Vector2i mousePixel = sf::Mouse::getPosition(launcher);
                    sf::Vector2f mousePos = launcher.mapPixelToCoords(mousePixel);

                    if (textFull.getGlobalBounds().contains(mousePos)) {
                        isFullscreen = true;
                        shouldStartGame = true;
                        launcher.close();
                    }

                    if (textWin.getGlobalBounds().contains(mousePos)) {
                        isFullscreen = false;
                        shouldStartGame = true;
                        launcher.close();
                    }
                }
            }
        }

        sf::Vector2i mousePos = sf::Mouse::getPosition(launcher);
        textFull.setFillColor(textFull.getGlobalBounds().contains(sf::Vector2f(mousePos)) ? sf::Color::Red : sf::Color::Yellow);
        textWin.setFillColor(textWin.getGlobalBounds().contains(sf::Vector2f(mousePos)) ? sf::Color::Red : sf::Color::Yellow);

        launcher.clear(sf::Color::Transparent);
        launcher.draw(SmenuSprite);
        launcher.draw(textFull);
        launcher.draw(textWin);
        launcher.display();
    }
}