#pragma once

#include <SFML/Graphics.hpp>
#include <exception>
#include <string>

class GameException: public std::exception{
protected:
    std::string text;
public:
    explicit GameException(std::string msg);

    [[nodiscard]] const char* what() const noexcept override;
};

class ResourceException: public GameException {
public:
    explicit ResourceException(const std::string& path);
};

class StateException: public GameException {
public:
    explicit StateException(const std::string& msg);
};

class MapEntityException: public GameException {
    sf::Vector2f position;

public:
    MapEntityException(const std::string& msg, sf::Vector2f position_);
    [[nodiscard]] sf::Vector2f getPos() const;
};

class InvalidActionException: public GameException {
public:
    InvalidActionException(const std::string& action, const std::string& reason);
};