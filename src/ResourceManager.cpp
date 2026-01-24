#include "ResourceManager.h"
#include "GameExceptions.h"

ResourceManager::ResourceManager() {
    m_textureHolder.load("assets/textures","enemy.png");
    m_textureHolder.load("assets/textures","samussheet.png");
    m_textureHolder.load("assets/textures","projectile.png");
    m_textureHolder.load("assets/textures","tile.png");
    m_textureHolder.load("assets/textures","backgroudtile.png");
    m_textureHolder.load("assets/textures/map","background.bmp");
    m_textureHolder.load("assets/textures","settingsmenu.png");
    m_textureHolder.load("assets/textures","hud.png");
    m_textureHolder.load("assets/textures","portal.png");
    m_textureHolder.load("assets/textures","enemyAgro.png");
    m_textureHolder.load("assets/textures","helperanimal.png");
    m_textureHolder.load("assets/textures","ammo.png");
    m_textureHolder.load("assets/textures","tier1Costume.png");
    m_textureHolder.load("assets/textures","tier2Costume.png");
    m_textureHolder.load("assets/textures","tier3Costume.png");


    m_soundHolder.load("assets/sound","jump.wav");
    m_soundHolder.load("assets/sound","reload.wav");
    m_soundHolder.load("assets/sound","shoot.wav");
    m_soundHolder.load("assets/sound","enemydeath.wav");
    m_soundHolder.load("assets/sound","enemydamage.wav");
    m_soundHolder.load("assets/sound","portalactive.wav");
    m_soundHolder.load("assets/sound","tier1.wav");
    m_soundHolder.load("assets/sound","pickupammo.wav");
    m_soundHolder.load("assets/sound","final_shot.wav");
    m_soundHolder.load("assets/sound","samus_flip.wav");

    m_fontHolder.load("assets/textures", "Metroid-Fusion.ttf");
}

ResourceManager& ResourceManager::Instance() {
    static ResourceManager instance;
    return instance;
}

void ResourceManager::cleanup() {
    m_textureHolder.clear();
    m_soundHolder.clear();
    m_fontHolder.clear();
}