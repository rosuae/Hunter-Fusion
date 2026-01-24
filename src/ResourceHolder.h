#ifndef OOP_RESOURCEHOLDER_H
#define OOP_RESOURCEHOLDER_H

#include <unordered_map>
#include <string>
#include <memory>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include "GameExceptions.h"

template <typename Resource>
class ResourceHolder {
    std::unordered_map<std::string, std::unique_ptr<Resource>> m_resources;
public:
    void load(const std::string& path, const std::string& filename) {
        auto resource = std::make_unique<Resource>();
        if (!resource->loadFromFile(path + "/" + filename)) {
            throw ResourceException("Resource: " + path + "/" + filename + " could not be loaded");
        }
        m_resources[filename] = std::move(resource);
    }

    Resource& get(const std::string& filename) {
        if (!m_resources.contains(filename)) {
            throw ResourceException("Resource: " + filename + " was not found");
        }
        return *m_resources[filename];
    }

    void clear() {
        m_resources.clear();
    }
};

template <>
inline void ResourceHolder<sf::Font>::load(const std::string& path, const std::string& filename) {
    auto font = std::make_unique<sf::Font>();
    if (!font->openFromFile(path + "/" + filename)) {
        throw ResourceException("Font: " + path + "/" + filename + " could not be loaded");
    }
    m_resources[filename] = std::move(font);
}

#endif //OOP_RESOURCEHOLDER_H