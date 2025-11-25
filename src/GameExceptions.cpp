#include "GameExceptions.h"
#include <iostream>

GameException::GameException(std::string msg) : text{std::move(msg)} {}

const char* GameException::what() const noexcept {
    return text.c_str();
}

ResourceException::ResourceException(const std::string &path) : GameException("Failed to load resource: " + path) {}

StateException::StateException(const std::string &msg) : GameException("Invalid state change: " + msg){}

MapEntityException::MapEntityException(const std::string& msg, const sf::Vector2f position_) : GameException("Entity behavior failed: " + msg), position(position_) {
    std::cout << "[SPAWN FAILED] ->  " << position.x << " : " << position.y << std::endl;
}

sf::Vector2f MapEntityException::getPos() const {
    return position;
}

InvalidActionException::InvalidActionException(const std::string& action, const std::string& reason) : GameException("[Action Denied] " + action + ": " + reason) {}