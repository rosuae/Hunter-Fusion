#pragma once

//
// #include <SFML/Graphics.hpp>
// class Game;
//
// class State {
// protected:
//     Game* m_game;
//
// public:
//     explicit State(Game* game) : m_game(game) {}
//     virtual ~State() = default;
//
//     void init() {
//         initImpl();
//     }
//
//     void handleInput(const sf::Event& event) {
//         inputImpl(event);
//     }
//
//     void update(const float deltaTime) {
//         updateImpl(deltaTime);
//     }
//
//     void draw(const float deltaTime) {
//         drawImpl(deltaTime);
//     }
//
//     void pause() { pauseImpl(); }
//     void resume() { resumeImpl(); }
//
// private:
//
//     virtual void initImpl() = 0;
//     virtual void inputImpl(const sf::Event& event) = 0;
//     virtual void updateImpl(float deltaTime) = 0;
//     virtual void drawImpl(float deltaTime) = 0;
//
//     virtual void pauseImpl() {}
//     virtual void resumeImpl() {}
// };