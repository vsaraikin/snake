#include "../src/Snake.hpp"

#include <catch2/catch_test_macros.hpp>

TEST_CASE("Snake reset positions at center", "[snake]") {
    Snake snake;
    snake.reset(20, 20);
    auto head = snake.head();
    CHECK(head.x == 10);
    CHECK(head.y == 10);
    CHECK(snake.body().size() == 3);
}

TEST_CASE("Snake reset with different grid sizes", "[snake]") {
    Snake snake;
    snake.reset(30, 30);
    CHECK(snake.head().x == 15);
    CHECK(snake.head().y == 15);

    snake.reset(15, 15);
    CHECK(snake.head().x == 7);
    CHECK(snake.head().y == 7);
}

TEST_CASE("Snake moves left by default", "[snake]") {
    Snake snake;
    snake.reset(20, 20);
    auto initial_head = snake.head();
    snake.update();
    CHECK(snake.head().x == initial_head.x - 1);
    CHECK(snake.head().y == initial_head.y);
}

TEST_CASE("Snake moves in all directions", "[snake]") {
    Snake snake;

    // Move right (need to go up first to avoid 180-degree block)
    snake.reset(20, 20);
    snake.set_direction(Direction::Up);
    snake.update();
    snake.set_direction(Direction::Right);
    snake.update();
    CHECK(snake.head().x == 11);

    // Move up
    snake.reset(20, 20);
    snake.set_direction(Direction::Up);
    snake.update();
    CHECK(snake.head().y == 9);

    // Move down
    snake.reset(20, 20);
    snake.set_direction(Direction::Down);
    snake.update();
    CHECK(snake.head().y == 11);
}

TEST_CASE("Snake prevents 180-degree turns", "[snake]") {
    Snake snake;
    snake.reset(20, 20);
    // Default direction is Left
    snake.set_direction(Direction::Right); // should be ignored
    snake.update();
    CHECK(snake.head().x == 9); // moved left, not right
}

TEST_CASE("Snake growth adds segment", "[snake]") {
    Snake snake;
    snake.reset(20, 20);
    CHECK(snake.body().size() == 3);

    snake.grow();
    snake.update();
    CHECK(snake.body().size() == 4);
}

TEST_CASE("Snake self-collision detection", "[snake]") {
    Snake snake;
    snake.reset(20, 20);

    // Grow enough to collide with self
    for (int i = 0; i < 5; ++i) {
        snake.grow();
        snake.update(); // moving left
    }
    // Now turn into itself
    snake.set_direction(Direction::Down);
    snake.update();
    snake.set_direction(Direction::Right);
    snake.update();
    snake.set_direction(Direction::Up);
    snake.update();

    CHECK(snake.has_self_collision());
}

TEST_CASE("Snake out-of-bounds detection", "[snake]") {
    Snake snake;
    snake.reset(20, 20);

    // Move left until out of bounds (head starts at x=10)
    for (int i = 0; i < 11; ++i) {
        snake.update();
    }
    CHECK(snake.is_out_of_bounds(20, 20));
}

TEST_CASE("Snake occupies checks all segments", "[snake]") {
    Snake snake;
    snake.reset(20, 20);
    // Head at (10,10), body at (11,10), (12,10)
    CHECK(snake.occupies({10, 10}));
    CHECK(snake.occupies({11, 10}));
    CHECK(snake.occupies({12, 10}));
    CHECK_FALSE(snake.occupies({0, 0}));
}

TEST_CASE("Snake prev_body is snapshot before update", "[snake]") {
    Snake snake;
    snake.reset(20, 20);
    auto body_before = snake.body();
    snake.update();
    CHECK(snake.prev_body() == body_before);
}
