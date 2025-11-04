#include "ResourceManager.h"

ResourceManager::ResourceManager() {
    loadTextures("assets/textures","enemy.png");
    loadTextures("assets/textures","samustest.png");
    loadTextures("assets/textures","projectile.png");
    loadTextures("assets/textures/map","background.bmp");
    loadTextures("assets/textures","tile.png");

    loadSounds("assets/sound","jump.wav");
    loadSounds("assets/sound","reload.wav");
    loadSounds("assets/sound","shoot.wav");
    loadSounds("assets/sound","enemydeath.wav");
    loadSounds("assets/sound","enemydamage.wav");
}

ResourceManager& ResourceManager::Instance() {
    static ResourceManager instance;
    return instance;
}

void ResourceManager::cleanup() {
    m_textures.clear();
    m_sounds.clear();
    std::cout << "ResourceManager curatat\n";
}

void ResourceManager::loadTextures(const std::string &path, const std::string &texture_name) {
    sf::Texture tex;

    if (!tex.loadFromFile(path + '/' + texture_name)) {
        throw std::runtime_error("Textura " + texture_name + " nu a putut fi incarcata.");
    }

    m_textures[texture_name] = tex;
}

void ResourceManager::loadSounds(const std::string &path, const std::string &sound_name) {
    sf::SoundBuffer sound;

    if (!sound.loadFromFile(path + '/' + sound_name)) {
        throw std::runtime_error("Sunetul " + sound_name + " nu a putut fi incarcat ");
    }

    m_sounds[sound_name] = sound;
}

sf::Texture& ResourceManager::getTexture (const std::string& texture_name) {
    if (m_textures.find(texture_name) == m_textures.end()) {
        throw std::runtime_error("Textura " + texture_name + " nu a fost gasita ");
    }

    return m_textures[texture_name];
}

sf::SoundBuffer& ResourceManager::getSound(const std::string& sound_name) {
    if (m_sounds.find(sound_name) == m_sounds.end()) {
        throw std::runtime_error("Sound ul " + sound_name + "nu a fost gasit");
    }

    return m_sounds[sound_name];
}