#include "ResourceManager.h"
#include "GameExceptions.h"

ResourceManager::ResourceManager() {
    loadTextures("assets/textures","enemy.png");
    loadTextures("assets/textures","samussheet.png");
    loadTextures("assets/textures","projectile.png");
    loadTextures("assets/textures","tile.png");
    loadTextures("assets/textures","backgroudtile.png");
    loadTextures("assets/textures/map","background.bmp");
    loadTextures("assets/textures","settingsmenu.png");
    loadTextures("assets/textures","hud.png");

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
}

void ResourceManager::loadTextures(const std::string &path, const std::string &texture_name) {
    sf::Texture tex;

    if (!tex.loadFromFile(path + '/' + texture_name)) {
        throw ResourceException("Texture: " + path + " couldn't be loaded");
    }

    m_textures[texture_name] = tex;
}

void ResourceManager::loadSounds(const std::string &path, const std::string &sound_name) {
    sf::SoundBuffer sound;

    if (!sound.loadFromFile(path + '/' + sound_name)) {
        throw ResourceException("Sound: " + path + " couldn't be loaded");
    }

    m_sounds[sound_name] = sound;
}

sf::Texture& ResourceManager::getTexture (const std::string& texture_name) {
    if (m_textures.find(texture_name) == m_textures.end()) {
        throw ResourceException("Texture: " + texture_name + " wasn't found");
    }

    return m_textures[texture_name];
}

sf::SoundBuffer& ResourceManager::getSound(const std::string& sound_name) {
    if (m_sounds.find(sound_name) == m_sounds.end()) {
        throw ResourceException("Sound: " + sound_name + " wasn't found");
    }

    return m_sounds[sound_name];
}