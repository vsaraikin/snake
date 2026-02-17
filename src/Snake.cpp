#include "Snake.hpp"

#include <algorithm>
#include <ranges>
#include <utility>

Snake::Snake() {
    reset(20, 20);
}

void Snake::reset(int grid_w, int grid_h) {
    body_.clear();
    const int cx = grid_w / 2;
    const int cy = grid_h / 2;
    body_.emplace_back(cx, cy);
    body_.emplace_back(cx + 1, cy);
    body_.emplace_back(cx + 2, cy);
    prev_body_ = body_;
    direction_ = Direction::Left;
    pending_direction_ = Direction::Left;
    should_grow_ = false;
}

void Snake::set_direction(Direction dir) {
    auto opposite = [](Direction a, Direction b) {
        return (a == Direction::Up && b == Direction::Down) ||
               (a == Direction::Down && b == Direction::Up) ||
               (a == Direction::Left && b == Direction::Right) ||
               (a == Direction::Right && b == Direction::Left);
    };

    if (!opposite(dir, direction_)) {
        pending_direction_ = dir;
    }
}

void Snake::update() {
    prev_body_ = body_;
    direction_ = pending_direction_;

    sf::Vector2i delta;
    switch (direction_) {
    case Direction::Up: delta = {0, -1}; break;
    case Direction::Down: delta = {0, 1}; break;
    case Direction::Left: delta = {-1, 0}; break;
    case Direction::Right: delta = {1, 0}; break;
    default: std::unreachable();
    }

    const sf::Vector2i new_head = head() + delta;
    body_.push_front(new_head);

    if (should_grow_) {
        should_grow_ = false;
    } else {
        body_.pop_back();
    }
}

bool Snake::has_self_collision() const {
    auto tail = std::ranges::subrange(body_.begin() + 1, body_.end());
    return std::ranges::contains(tail, head());
}

bool Snake::is_out_of_bounds(int width, int height) const {
    const auto h = head();
    return h.x < 0 || h.x >= width || h.y < 0 || h.y >= height;
}

bool Snake::occupies(sf::Vector2i pos) const {
    return std::ranges::contains(body_, pos);
}

void Snake::grow() {
    should_grow_ = true;
}
