#pragma once

#include <SFML/Graphics/Color.hpp>

#include <chrono>

struct Config {
    // Grid (defaults, overridden by Settings)
    static constexpr int grid_width = 20;
    static constexpr int grid_height = 20;
    static constexpr int cell_size = 32;

    // Window
    static constexpr int window_width = grid_width * cell_size;
    static constexpr int window_height = grid_height * cell_size;
    static constexpr const char* window_title = "Snake";

    // Timing
    static constexpr auto initial_tick = std::chrono::milliseconds{150};
    static constexpr auto min_tick = std::chrono::milliseconds{60};
    static constexpr int speed_increment_score = 3;

    // Colors
    static inline const sf::Color background{30, 30, 46};
    static inline const sf::Color grid_line{45, 45, 65};
    static inline const sf::Color snake_head{166, 227, 161};
    static inline const sf::Color snake_body{116, 199, 136};
    static inline const sf::Color food_color{243, 139, 168};
    static inline const sf::Color text_color{205, 214, 244};
    static inline const sf::Color overlay_bg{30, 30, 46, 200};
    static inline const sf::Color bonus_food_color{250, 200, 50};

    // Bonus food
    static constexpr float bonus_spawn_chance = 0.3f;
    static constexpr float bonus_duration = 5.0f;
    static constexpr int bonus_points = 5;

    // Screen shake
    static constexpr float shake_duration = 0.3f;
    static constexpr float shake_intensity = 6.0f;

    // Font
    static constexpr const char* font_path = "assets/fonts/JetBrainsMono-Regular.ttf";
};
