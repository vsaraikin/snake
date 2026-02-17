#pragma once

#include <SFML/System/Vector2.hpp>

#include <deque>

enum class Direction { Up, Down, Left, Right };

class Snake {
public:
    Snake();

    void reset(int grid_w, int grid_h);
    void set_direction(Direction dir);
    void update();
    [[nodiscard]] bool has_self_collision() const;
    [[nodiscard]] bool is_out_of_bounds(int width, int height) const;
    [[nodiscard]] bool occupies(sf::Vector2i pos) const;
    void grow();

    [[nodiscard]] const std::deque<sf::Vector2i>& body() const { return body_; }
    [[nodiscard]] const std::deque<sf::Vector2i>& prev_body() const { return prev_body_; }
    [[nodiscard]] sf::Vector2i head() const { return body_.front(); }
    [[nodiscard]] Direction direction() const { return direction_; }

private:
    std::deque<sf::Vector2i> body_;
    std::deque<sf::Vector2i> prev_body_;
    Direction direction_ = Direction::Left;
    Direction pending_direction_ = Direction::Left;
    bool should_grow_ = false;
};
