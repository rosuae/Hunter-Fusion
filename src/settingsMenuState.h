#pragma once
#include "ResourceManager.h"

class settingsMenuState {

    bool isFullscreen;
    unsigned int width;
    unsigned int height;
    bool shouldStartGame;

    sf::Texture texture;
    sf::Sprite SmenuSprite;
    sf::RenderWindow launcher;
public:
    settingsMenuState();
    void SetSettingsLauncher();

    [[nodiscard]] auto GetWindowStyle() const {
        return isFullscreen ? sf::State::Fullscreen : sf::State::Windowed;
    }

    sf::Vector2u GetResolution() const{
        return {width, height};
    }
};