#ifndef OOP_RESOURCEMANAGER_H
#define OOP_RESOURCEMANAGER_H

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include "ResourceHolder.h"

class ResourceManager {
public:
    ResourceManager(const ResourceManager&) = delete;

    static ResourceManager& Instance();

    void cleanup();

    ResourceHolder<sf::Texture>& textures() { return m_textureHolder; }
    ResourceHolder<sf::SoundBuffer>& sounds() { return m_soundHolder; }
    ResourceHolder<sf::Font>& fonts() { return m_fontHolder; }

private:
    ResourceManager();

    ResourceHolder<sf::Texture> m_textureHolder;
    ResourceHolder<sf::SoundBuffer> m_soundHolder;
    ResourceHolder<sf::Font> m_fontHolder;
};

#endif //OOP_RESOURCEMANAGER_H