#pragma once

#include "Config.hpp"
#include "Snake.hpp"

#include <SFML/System/Vector2.hpp>

#include <chrono>
#include <optional>
#include <random>

class Board {
public:
    Board(int grid_w, int grid_h);

    void spawn_food(const Snake& snake);
    void spawn_bonus(const Snake& snake);
    void update_bonus(float dt);
    void clear_bonus();

    [[nodiscard]] sf::Vector2i food_position() const { return food_; }
    [[nodiscard]] std::optional<sf::Vector2i> bonus_position() const { return bonus_pos_; }
    [[nodiscard]] float bonus_time_remaining() const { return bonus_timer_; }

    [[nodiscard]] int width() const { return grid_w_; }
    [[nodiscard]] int height() const { return grid_h_; }

    static sf::Vector2f grid_to_pixel(sf::Vector2i grid_pos, int cell_size);

private:
    int grid_w_;
    int grid_h_;
    sf::Vector2i food_;
    std::optional<sf::Vector2i> bonus_pos_;
    float bonus_timer_ = 0.f;
    std::mt19937 rng_;
};
