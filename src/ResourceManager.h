#ifndef OOP_RESOURCEMANAGER_H
#define OOP_RESOURCEMANAGER_H

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <unordered_map>

class ResourceManager {
public:
    ResourceManager(const ResourceManager&) = delete;

    static ResourceManager& Instance();

    void cleanup();

    sf::Texture& getTexture(const std::string& texture_name);
    sf::SoundBuffer& getSound(const std::string& sound_name);
    sf::Font& getFont(const std::string& font_name);

private:
    ResourceManager();
    ~ResourceManager() = default;


    void loadTextures(const std::string& path, const std::string& texture_name);
    void loadSounds(const std::string& path, const std::string& sound_path);
    void loadFonts(const std::string& path, const std::string& font_name);

    std::unordered_map<std::string, sf::Texture> m_textures;
    std::unordered_map<std::string, sf::SoundBuffer> m_sounds;
    std::unordered_map<std::string, sf::Font> m_fonts;
};

#endif //OOP_RESOURCEMANAGER_H