#pragma once

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>

class gameState {
public:

    gameState() :
    soundbuffer{"assets/sound/reload.wav"},
    sound{soundbuffer}
    {}

    virtual ~gameState() = default;

    virtual void handleEvents() = 0;
    virtual void update() = 0;
    virtual void draw() = 0;

protected:

    sf::SoundBuffer soundbuffer;
    sf::Sound sound;
};
