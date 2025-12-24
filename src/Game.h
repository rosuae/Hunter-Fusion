#pragma once

#include <list>
#include <map>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>

#include "settingsMenuState.h"
#include "Camera.h"

class Player;
class Pet;
class Map;
class settingsMenuState;
class Camera;

enum class GameState {
    MainMenu,
    Playing,
    Paused,
    GameOver
};

class Game {
    void renderUI(sf::RenderWindow& window);

    void handleInputMenu(const sf::Event& event);
    void handleInputPlaying(const sf::Event& event);
    void handleInputPaused(const sf::Event& event);
    void handleInputGameOver(const sf::Event& event);

    void instanceObjects();
    void initPlayer(const std::string& playerName, const std::string& weaponName, const std::string& projName);
    void managePetSpawn() const;
    void loadLevel(const std::string& mapFile, sf::Vector2f spawnPos = sf::Vector2f(-1.f, -1.f));
    void loadLevel(const std::pair<std::string, sf::Vector2f>& nextDestination);
    void handleEvents();
    void update(float deltaTime);
    void render();

    void handleCollisions();
    void updateCamera(float deltaTime) const;
    void updateSounds();
    void resetGame();
    void respawnPlayer();

    ResourceManager& m_resManager;
    settingsMenuState settings;
    GameState m_state = GameState::MainMenu;
    sf::Text m_uiText;
    sf::RenderWindow m_window;
    std::unique_ptr<Camera> m_camera;
    unsigned int m_width;
    unsigned int m_height;

    std::unique_ptr<Player> m_player;
    std::unique_ptr<Pet> m_pet;
    std::unique_ptr<Map> m_map;
    std::string m_lastMapPath;
    sf::Vector2f m_lastSpawnPos;

    sf::Clock m_clock;
    sf::Clock m_playerDamageCooldown;

    std::list <sf::Sound> m_playingSounds;
    std::string m_currentMapPath;
    std::map<std::string, std::unique_ptr<Map>> m_savedMaps;
public:
    Game();
    ~Game();
    void run();
    static int generateRandomInt(int min, int max);
};