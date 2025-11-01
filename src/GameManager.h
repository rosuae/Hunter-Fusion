#ifndef OOP_GAMEMANAGER_H
#define OOP_GAMEMANAGER_H

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>

class GameManager {
    sf::Texture projectileTex;
    sf::Texture playerTex;
    sf::Texture backgroundTex;
    sf::Texture enemyTex;

    sf::SoundBuffer shootBuffer;
    sf::SoundBuffer jumpBuffer;
    sf::SoundBuffer reloadBuffer;
    sf::SoundBuffer enemyDeathBuffer;
    sf::SoundBuffer enemyDamageBuffer;

    sf::RenderWindow window;
};

#endif