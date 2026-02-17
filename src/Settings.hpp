#pragma once

#include "Config.hpp"

#include <SFML/Window/Keyboard.hpp>

#include <chrono>
#include <string>

struct KeyBindings {
    sf::Keyboard::Key up = sf::Keyboard::Key::Up;
    sf::Keyboard::Key down = sf::Keyboard::Key::Down;
    sf::Keyboard::Key left = sf::Keyboard::Key::Left;
    sf::Keyboard::Key right = sf::Keyboard::Key::Right;
    sf::Keyboard::Key pause = sf::Keyboard::Key::P;
};

struct Settings {
    int grid_size = 20;                            // 15, 20, 25, 30
    std::chrono::milliseconds starting_speed{150}; // 200, 150, 100
    KeyBindings keys;

    void load();
    void save() const;

    [[nodiscard]] int window_size() const { return grid_size * Config::cell_size; }

    static std::string key_to_name(sf::Keyboard::Key key);
    static sf::Keyboard::Key name_to_key(const std::string& name);

    // Speed presets
    static constexpr int speed_slow = 200;
    static constexpr int speed_medium = 150;
    static constexpr int speed_fast = 100;
};
