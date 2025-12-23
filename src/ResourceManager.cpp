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
    loadTextures("assets/textures","portal.png");
    loadTextures("assets/textures","enemyAgro.png");
    loadTextures("assets/textures","helperanimal.png");
    loadTextures("assets/textures","tier1Costume.png");


    loadSounds("assets/sound","jump.wav");
    loadSounds("assets/sound","reload.wav");
    loadSounds("assets/sound","shoot.wav");
    loadSounds("assets/sound","enemydeath.wav");
    loadSounds("assets/sound","enemydamage.wav");
    loadSounds("assets/sound","portalactive.wav");
    loadSounds("assets/sound","tier1.wav");

    loadFonts("assets/textures", "Metroid-Fusion.ttf");
}

ResourceManager& ResourceManager::Instance() {
    static ResourceManager instance;
    return instance;
}

void ResourceManager::cleanup() {
    m_textures.clear();
    m_sounds.clear();
    m_fonts.clear();
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

void ResourceManager::loadFonts(const std::string &path, const std::string &font_name) {
    sf::Font font;

    if (!font.openFromFile(path + '/' + font_name)) {
        throw ResourceException("Font: " + path + '/' + font_name + " couldn't be loaded");
    }

    m_fonts[font_name] = font;
}

sf::Texture& ResourceManager::getTexture (const std::string& texture_name) {
    if (!m_textures.contains(texture_name)) {
        throw ResourceException("Texture: " + texture_name + " wasn't found");
    }

    return m_textures[texture_name];
}

sf::SoundBuffer& ResourceManager::getSound(const std::string& sound_name) {
    if (!m_sounds.contains(sound_name)) {
        throw ResourceException("Sound: " + sound_name + " wasn't found");
    }

    return m_sounds[sound_name];
}

sf::Font& ResourceManager::getFont(const std::string& font_name) {
    if (!m_fonts.contains(font_name)) {
        throw ResourceException("Font: " + font_name + " wasn't found");
    }

    return m_fonts[font_name];
}