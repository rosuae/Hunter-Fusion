#ifndef OOP_RESOURCEMANAGER_H
#define OOP_RESOURCEMANAGER_H

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <unordered_map>
#include <iostream>

class ResourceManager {
public:
    ResourceManager(const ResourceManager&) = delete;

    static ResourceManager& Instance();

    void cleanup();

    sf::Texture& getTexture(const std::string& texture_name);
    sf::SoundBuffer& getSound(const std::string& sound_name);

private:
    ResourceManager();
    ~ResourceManager() = default;


    void loadTextures(const std::string& path, const std::string& texture_name);
    void loadSounds(const std::string& path, const std::string& sound_path);

    std::unordered_map<std::string, sf::Texture> m_textures;
    std::unordered_map<std::string, sf::SoundBuffer> m_sounds;
};

#endif //OOP_RESOURCEMANAGER_H