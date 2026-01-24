#include "Game.h"
#include "Portal.h"
#include "Enemy.h"
#include "Player.h"
#include "Pet.h"
#include "Map.h"
#include "settingsMenuState.h"
#include "Camera.h"
#include "ResourceManager.h"
#include "GameExceptions.h"
#include "Utils.h"
#include <fstream>
#include <random>

void Game::instanceObjects() {
    std::ifstream fin("date.txt");
    if (!fin.is_open()) {
        throw ResourceException("Couldn't load file: date.txt");
    }

    std::string playerName, playerWeapon, projectileName;
    if (!(fin >> playerName >> playerWeapon >> projectileName)) {
        throw ResourceException("File: date.txt ; incomplete or currupted");
    }

    try {
        m_resManager.textures().get("ammo.png");
    } catch (const ResourceException& e) {
        std::cout << "Warning: Failed to preload pickup textures: " << e.what() << std::endl;
    }

    fin.close();

    std::string initialMapPath = "assets/textures/map/harta.txt";
    m_currentMapPath = initialMapPath;

    auto tempMap = std::make_unique<Map>(
            "CurrentRoom",
            initialMapPath,
            m_resManager,
            m_playingSounds
        );
    m_map = std::move(tempMap);

    initPlayer(playerName, playerWeapon, projectileName);

    m_map->initializeWithExistingPlayer(*m_player);

    managePetSpawn();

    m_window.create(sf::VideoMode({m_width, m_height}), "Hunter Fusion", settings.GetWindowStyle());
    m_window.setFramerateLimit(90);
    // m_window.setVerticalSyncEnabled(true);

    m_camera = std::make_unique<Camera>(m_width, m_height, *m_player, m_resManager);
    m_camera->snapToPlayer();
    m_camera->updateMinimap(m_map->getLayout());

    m_lastMapPath = initialMapPath;
    m_lastSpawnPos = m_player->getPos();
}

void Game::initPlayer(const std::string& playerName, const std::string& weaponName, const std::string& projName) {
    auto tempWeapon = std::make_unique<Weapon>(
        weaponName, projName, 50,
        m_resManager.textures().get("projectile.png"), 60,
        m_playingSounds, m_resManager.sounds().get("shoot.wav"), m_resManager.sounds().get("reload.wav")
    );

    m_player = std::make_unique<Player>(
        playerName,
        m_resManager.textures().get("samussheet.png"),
        0.f, 0.f,
        m_playingSounds,
        m_resManager.sounds().get("jump.wav"),
        m_resManager.sounds().get("final_shot.wav"),
        m_resManager.sounds().get("samus_flip.wav"),
        std::move(tempWeapon)
    );

    m_map->placeEntity(*m_player, 'P');

    try {
        m_player->addSkinUnlock(1000, m_resManager.textures().get("tier1Costume.png"), m_resManager.sounds().get("tier1.wav"));
        m_player->addSkinUnlock(6000, m_resManager.textures().get("tier2Costume.png"), m_resManager.sounds().get("tier1.wav"));
        m_player->addSkinUnlock(10000, m_resManager.textures().get("tier3Costume.png"), m_resManager.sounds().get("tier1.wav"));
    }
    catch (const ResourceException& e) {
        std::cout << "Warning: Could not load upgrade skins: " << e.what() << "\n";
    }
}

void Game::managePetSpawn() const {
    if (m_pet) {
        m_map->placeEntity(*m_pet, 'C');
        return;
    }

    if (m_map->hasPet()) {
        return;
    }

    auto [x, y] = m_map->findSpawnLocation('C');

    if (x >= 0 && y >= 0) {
        try {
            auto tempPet = std::make_unique<Pet>(
                "Companion",
                0.0f, 0.0f,
                m_resManager.textures().get("helperanimal.png"),
                nullptr
            );

            tempPet->spawnAt(x, y);
            m_map->spawnEntityAt(std::move(tempPet));

        } catch (const ResourceException& e) {
            std::cout << "Warning: Map has 'C' tag but pet failed to load: " << e.what() << "\n";
        }
    }
}

void Game::loadLevel(const std::string& mapFile, const sf::Vector2f spawnPos) {
    auto tempMap = std::make_unique<Map>(
        "CurrentRoom",
        mapFile,
        m_resManager,
        m_playingSounds
    );

    m_map = std::move(tempMap);

    if (m_player) {
        if (spawnPos.x < 0) {
            m_map->initializeWithPlayer(*m_player);
        } else {
            m_player->spawnAt(spawnPos.x, spawnPos.y);
            m_map->initializeWithExistingPlayer(*m_player);
        }

        if (m_camera) {
            m_camera->snapToPlayer();
        }
    }

    managePetSpawn();

    m_clock.restart();
}

void Game::loadLevel(const std::pair<std::string, sf::Vector2f>& nextDestination) {
    std::string nextMapPath = nextDestination.first;
    const sf::Vector2f spawnPos = nextDestination.second;

    if (m_map) {
        if (!m_currentMapPath.empty()) {
            m_savedMaps[m_currentMapPath] = std::move(m_map);
        }
    }

    if (const auto it = m_savedMaps.find(nextMapPath); it != m_savedMaps.end()) {
        m_map = std::move(it->second);
        m_savedMaps.erase(it);
    }
    else {
        auto tempMap = std::make_unique<Map>(
            "CurrentRoom",
            nextMapPath,
            m_resManager,
            m_playingSounds
        );
        m_map = std::move(tempMap);
    }

    m_currentMapPath = nextMapPath;

    if (m_player) {
        m_player->resetWeaponProjectiles();
        if (spawnPos.x < 0) {
            m_map->initializeWithExistingPlayer(*m_player);
        } else {
            m_player->spawnAt(spawnPos.x, spawnPos.y);
            m_map->initializeWithExistingPlayer(*m_player);
        }
        if (m_camera) {
            m_camera->snapToPlayer();
            m_camera->updateMinimap(m_map->getLayout());
        }
    }

    managePetSpawn();

    m_clock.restart();
}

Game::Game() :
    m_resManager{ResourceManager::Instance()},
    m_uiText{m_resManager.fonts().get("Metroid-Fusion.ttf")},
    m_width{},
    m_height{} {
    settings.SetSettingsLauncher();
    m_width = settings.GetResolution().x;
    m_height = settings.GetResolution().y;
}

Game::~Game() {
    for (auto& sound : m_playingSounds) {
        sound.stop();
    }
    m_map.reset();
    m_playingSounds.clear();
    m_resManager.cleanup();
}

void Game::run() {
    instanceObjects();
    m_state = GameState::MainMenu;

    m_clock.restart();
    while (m_window.isOpen()) {
        float deltaTime = m_clock.restart().asSeconds();
        if (deltaTime > 0.1f) deltaTime = 0.1f;
        handleEvents();
        if (m_state == GameState::Playing) {
            update(deltaTime);

            if (m_player && !m_player->isAlive()) {
                m_state = GameState::GameOver;
            }
        }
        render();
    }
}

void Game::handleEvents() {
    while (const std::optional<sf::Event> event = m_window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) {
            m_window.close();
        }
        switch (m_state) {
            case GameState::MainMenu:handleInputMenu(*event); break;
            case GameState::Playing:handleInputPlaying(*event); break;
            case GameState::Paused:handleInputPaused(*event); break;
            case GameState::GameOver:handleInputGameOver(*event); break;
        }
    }
}

void Game::handleInputMenu(const sf::Event& event) {
    if (const auto* keyPress = event.getIf<sf::Event::KeyPressed>()) {
        if (keyPress->scancode == sf::Keyboard::Scancode::W ||
            keyPress->scancode == sf::Keyboard::Scancode::Up) {
            m_menuSelection--;
            if (m_menuSelection < 0) m_menuSelection = 2; }

        else if (keyPress->scancode == sf::Keyboard::Scancode::S ||
                 keyPress->scancode == sf::Keyboard::Scancode::Down) {
            m_menuSelection++;
            if (m_menuSelection > 2) m_menuSelection = 0; }

        if (keyPress->scancode == sf::Keyboard::Scancode::Enter) {
            if (m_menuSelection == 0) {
                resetGame();
                m_isSessionActive = true;
                m_state = GameState::Playing;
            }
            else if (m_menuSelection == 1) {
                if (m_isSessionActive && m_player && m_player->isAlive()) {
                    m_state = GameState::Playing;
                    m_clock.restart();
                }
            }
            else if (m_menuSelection == 2) {
                m_window.close();
            }
        }
        else if (keyPress->scancode == sf::Keyboard::Scancode::Escape) {
            m_window.close();
        }
    }
}

void Game::handleInputPlaying(const sf::Event &event) {
    if (const auto* keyPress = event.getIf<sf::Event::KeyPressed>()) {
        if (keyPress->scancode == sf::Keyboard::Scancode::Escape) {
            m_state = GameState::Paused;
        }
    }
}

void Game::handleInputPaused(const sf::Event& event) {
    if (const auto* keyPress = event.getIf<sf::Event::KeyPressed>()) {
        if (keyPress->scancode == sf::Keyboard::Scancode::Escape) {
            m_state = GameState::Playing;
            m_clock.restart();
        }
        else if (keyPress->scancode == sf::Keyboard::Scancode::M) {
            m_state = GameState::MainMenu;
            m_menuSelection = 0;
        }
    }
}

void Game::handleInputGameOver(const sf::Event& event) {
    if (const auto* keyPress = event.getIf<sf::Event::KeyPressed>()) {
        if (keyPress->scancode == sf::Keyboard::Scancode::R) {
            respawnPlayer();
        }
        else if (keyPress->scancode == sf::Keyboard::Scancode::Escape) {
            m_isSessionActive = false;
            resetGame();
            m_state = GameState::MainMenu;
        }
    }
}

void Game::update(const float deltaTime) {
    m_player->behavior(deltaTime, *m_map);
    if (m_pet) {
        m_pet->behavior(deltaTime, *m_map);
    }

    m_map->updateEntities(deltaTime);
    handleCollisions();
    updateSounds();
    updateCamera(deltaTime);
}

void Game::handleCollisions() {
    m_map->processProjectileCollisions();
    m_map->handleCollisions(*m_player);

    if (Entity* petPtr = m_player->takePendingPet()) {
        if (std::unique_ptr<Entity> transferedEntity = m_map->claimEntity(petPtr)) {
            m_pet.reset(dynamic_cast<Pet*>(transferedEntity.release()));
            m_pet->setOwner(m_player.get());
        }
    }

    if (const auto dest = m_player->consumeTeleportRequest()) {
        loadLevel(*dest);
        return;
    }

    m_map->cleanupAndRespawn();
}

void Game::updateCamera(const float deltaTime) const {
    if (!m_player) return;
    m_camera->followPlayer(deltaTime);
}

void Game::updateSounds() {
    m_playingSounds.remove_if([](const sf::Sound& Sound_) {
        return Sound_.getStatus() == sf::Sound::Status::Stopped;
    });
}

void Game::resetGame() {
    for (auto& sound : m_playingSounds) {
        sound.stop();
    }

    m_playingSounds.clear();
    m_camera.reset();
    m_savedMaps.clear();
    m_map.reset();
    m_pet.reset();
    m_player.reset();

    std::ifstream fin("date.txt");
    if (!fin.is_open()) {
        throw ResourceException("Couldn't load file: date.txt");
    }

    std::string playerName, playerWeapon, projectileName;
    if (!(fin >> playerName >> playerWeapon >> projectileName)) {
        return;
    }
    fin.close();

    std::string initialMapPath = "assets/textures/map/harta.txt";
    m_currentMapPath = initialMapPath;
    m_lastMapPath = initialMapPath;

    auto tempMap = std::make_unique<Map>(
            "CurrentRoom",
            initialMapPath,
            m_resManager,
            m_playingSounds
        );
    m_map = std::move(tempMap);

    initPlayer(playerName, playerWeapon, projectileName);

    m_lastSpawnPos = m_player->getPos();

    m_map->initializeWithExistingPlayer(*m_player);
    managePetSpawn();

    m_camera = std::make_unique<Camera>(m_width, m_height, *m_player, m_resManager);
    m_camera->snapToPlayer();
    m_camera->updateMinimap(m_map->getLayout());

    m_clock.restart();
    m_playerDamageCooldown.restart();
}

void Game::respawnPlayer() {
    loadLevel({m_lastMapPath, m_lastSpawnPos});

    if (m_player) {
        m_player->resurrect();
    }

    m_state = GameState::Playing;
    m_clock.restart();
}

void Game::render() {
    m_window.clear(sf::Color::Black);

    if (m_state != GameState::MainMenu) {
        if (m_map && m_player) {
            m_camera->prepareScene(m_window);
            m_map->drawMap(m_window);
            m_player->draw(m_window);
            if (m_pet) {
                m_pet->draw(m_window);
            }

            m_map->drawEntities(m_window);
            m_camera->drawHud(m_window);
        }
    }
    m_window.setView(m_window.getDefaultView());
    renderUI(m_window);

    m_window.display();
}

void Game::renderUI(sf::RenderWindow& window) {
    if (m_state == GameState::Playing)
        return;

    m_uiText.setString("");

    sf::RectangleShape overlay(sf::Vector2f(static_cast<float>(m_width), static_cast<float>(m_height)));


    if (m_state == GameState::MainMenu) {
        m_uiText.setString("HUNTER FUSION");
        m_uiText.setCharacterSize(60);
        m_uiText.setFillColor(sf::Color::White);

        sf::FloatRect textRect = m_uiText.getLocalBounds();
        m_uiText.setOrigin({textRect.position.x + textRect.size.x / 2.0f, textRect.position.y + textRect.size.y / 2.0f});
        m_uiText.setPosition({static_cast<float>(m_width) / 2.0f, static_cast<float>(m_height) * 0.15f});
        window.draw(m_uiText);

        const std::vector<std::string> options = { "NEW GAME", "CONTINUE", "QUIT" };
        m_uiText.setCharacterSize(40);

        const float startY = static_cast<float>(m_height) * 0.35f;

        for (int i = 0; i < static_cast<int>(options.size()); ++i) {
            m_uiText.setString(options[i]);

            if (const bool isContinueOption = i == 1; isContinueOption && (!m_isSessionActive || !m_player || !m_player->isAlive())) {
                m_uiText.setFillColor(sf::Color(100, 100, 100));
            }
            else if (i == m_menuSelection) {
                m_uiText.setFillColor(sf::Color::Yellow);
            } else {
                m_uiText.setFillColor(sf::Color::White);
            }

            textRect = m_uiText.getLocalBounds();
            m_uiText.setOrigin({textRect.position.x + textRect.size.x / 2.0f, textRect.position.y + textRect.size.y / 2.0f});
            m_uiText.setPosition({static_cast<float>(m_width) / 2.0f, startY + static_cast<float>(i) * 60.0f});

            window.draw(m_uiText);
        }

        m_uiText.setString("1k bounty - tier 1 upgrade \n\n 6k bounty - tier 2 upgrade \n\n 10k bounty - tier 3 upgrade \n\nFind the helper animal");
        m_uiText.setCharacterSize(30);
        m_uiText.setFillColor(sf::Color::White);

        textRect = m_uiText.getLocalBounds();
        m_uiText.setOrigin({textRect.position.x + textRect.size.x / 2.0f, textRect.position.y + textRect.size.y / 2.0f});
        m_uiText.setPosition({static_cast<float>(m_width) / 2.0f, static_cast<float>(m_height) * 0.75f});
        window.draw(m_uiText);

        m_uiText.setCharacterSize(20);
        m_uiText.setFillColor(sf::Color(200, 200, 200));
        m_uiText.setString("W/S or Arrows to Navigate then Enter to Select");

        textRect = m_uiText.getLocalBounds();
        m_uiText.setOrigin({textRect.position.x + textRect.size.x / 2.0f, textRect.position.y + textRect.size.y / 2.0f});
        m_uiText.setPosition({static_cast<float>(m_width) / 2.0f, static_cast<float>(m_height) - 50.0f});
        window.draw(m_uiText);
    }
    else if (m_state == GameState::Paused) {
        overlay.setFillColor(sf::Color(0, 0, 0, 150));
        window.draw(overlay);

        m_uiText.setString("PAUSED\n\nResume - ESCAPE\nMain Menu - M");
        m_uiText.setCharacterSize(40);
        m_uiText.setFillColor(sf::Color::White);

        const sf::FloatRect textRect = m_uiText.getLocalBounds();
        m_uiText.setOrigin({textRect.position.x + textRect.size.x / 2.0f, textRect.position.y + textRect.size.y / 2.0f});
        m_uiText.setPosition({static_cast<float>(m_width) / 2.0f, static_cast<float>(m_height) / 2.0f});
        window.draw(m_uiText);
    }
    else if (m_state == GameState::GameOver) {
        overlay.setFillColor(sf::Color(150, 0, 0, 100));
        window.draw(overlay);

        m_uiText.setString("YOU DIED\n\nRetry - R\nMain Menu - Escape");
        m_uiText.setCharacterSize(40);
        m_uiText.setFillColor(sf::Color::White);

        const sf::FloatRect textRect = m_uiText.getLocalBounds();
        m_uiText.setOrigin({textRect.position.x + textRect.size.x / 2.0f, textRect.position.y + textRect.size.y / 2.0f});
        m_uiText.setPosition({static_cast<float>(m_width) / 2.0f, static_cast<float>(m_height) / 2.0f});
        window.draw(m_uiText);
    }
}