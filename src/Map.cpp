#include "Map.h"
#include "Player.h"
#include "Enemy.h"
#include "ResourceManager.h"

Map::Map(std::string n, Player& p, const std::string& filePath, ResourceManager& resManager):
    MapNume{std::move(n)},
    MyPlayer{p},
    tileSprite{resManager.getTexture("tile.png")} {

    std::ifstream file(filePath);
    if (!file.is_open()) {
        throw std::runtime_error("Eroare la deschiderea fisierului cu harta: " + filePath);
    }

    std::string line;
    while (std::getline(file, line)) {
        mapLayout.push_back(line);
    }

    file.close();

    for (const auto& mapLay : mapLayout) {
        std::cout << mapLay << "\n";
    }

    std::cout<<"Constructor Map\n";
}

void Map::drawMap(sf::RenderWindow& window) {

    for (size_t y = 0; y < mapLayout.size(); ++y)
        for (size_t x = 0; x <  mapLayout[y].size(); ++x) {
            char tileType = mapLayout[y][x];

            if (tileType == '#') {
                tileSprite.setPosition(sf::Vector2f (static_cast<float>(x) * TILE_SIZE, static_cast<float> (y) * TILE_SIZE));
                window.draw(tileSprite);
            }
        }
}

void Map::addEnemy(const Enemy& enemy) {
    enemies.push_back(enemy);
}

Map::~Map() { std::cout << "S a apelat destructor Map \n";}

bool Map::isWall(const sf::FloatRect& bounds) const {
    int startX = static_cast<int> (bounds.position.x / TILE_SIZE);
    int startY = static_cast<int> (bounds.position.y / TILE_SIZE);
    int endX = static_cast<int> ((bounds.position.x + bounds.size.x) / TILE_SIZE);
    int endY = static_cast<int> ((bounds.position.y + bounds.size.y) / TILE_SIZE);

    for (int y = startY; y <= endY; ++y) {
        if (y < 0 || y >= static_cast<int>(mapLayout.size()))
            continue;
        for (int x = startX; x <=  endX; ++x) {
            if (x < 0 || x >= static_cast<int>(mapLayout[y].size()))
                continue;
            if (mapLayout[y][x] == '#') {
                sf::FloatRect tileBounds(
                    sf::Vector2f(static_cast<float>(x) * TILE_SIZE, static_cast<float>(y) * TILE_SIZE),
                    sf::Vector2f(TILE_SIZE, TILE_SIZE)
                    );
                if (tileBounds.findIntersection(bounds).has_value())
                    return true;
            }
        }
    }
    return false;
}

std::vector<Enemy>& Map::getEnemies() {
    return enemies;
}