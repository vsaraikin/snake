#include "Board.hpp"

Board::Board(int grid_w, int grid_h)
    : grid_w_(grid_w), grid_h_(grid_h), rng_(std::random_device{}()) {
    food_ = {grid_w / 4, grid_h / 4};
}

void Board::spawn_food(const Snake& snake) {
    std::uniform_int_distribution<int> dist_x(0, grid_w_ - 1);
    std::uniform_int_distribution<int> dist_y(0, grid_h_ - 1);

    int attempts = grid_w_ * grid_h_;
    do {
        food_ = {dist_x(rng_), dist_y(rng_)};
    } while ((snake.occupies(food_) || (bonus_pos_ && food_ == *bonus_pos_)) && --attempts > 0);
}

void Board::spawn_bonus(const Snake& snake) {
    std::uniform_int_distribution<int> dist_x(0, grid_w_ - 1);
    std::uniform_int_distribution<int> dist_y(0, grid_h_ - 1);

    sf::Vector2i pos;
    int attempts = grid_w_ * grid_h_;
    do {
        pos = {dist_x(rng_), dist_y(rng_)};
    } while ((snake.occupies(pos) || pos == food_) && --attempts > 0);

    bonus_pos_ = pos;
    bonus_timer_ = Config::bonus_duration;
}

void Board::update_bonus(float dt) {
    if (!bonus_pos_) return;
    bonus_timer_ -= dt;
    if (bonus_timer_ <= 0.f) { // NOLINT(readability-use-std-min-max)
        bonus_pos_.reset();
        bonus_timer_ = 0.f;
    }
}

void Board::clear_bonus() {
    bonus_pos_.reset();
    bonus_timer_ = 0.f;
}

sf::Vector2f Board::grid_to_pixel(sf::Vector2i grid_pos, int cell_size) {
    return {static_cast<float>(grid_pos.x * cell_size), static_cast<float>(grid_pos.y * cell_size)};
}
