#include "../src/Board.hpp"

#include <catch2/catch_test_macros.hpp>

TEST_CASE("Board grid_to_pixel conversion", "[board]") {
    auto pixel = Board::grid_to_pixel({0, 0}, 32);
    CHECK(pixel.x == 0.f);
    CHECK(pixel.y == 0.f);

    pixel = Board::grid_to_pixel({5, 3}, 32);
    CHECK(pixel.x == 160.f);
    CHECK(pixel.y == 96.f);

    pixel = Board::grid_to_pixel({1, 1}, 16);
    CHECK(pixel.x == 16.f);
    CHECK(pixel.y == 16.f);
}

TEST_CASE("Board food spawns within bounds", "[board]") {
    Snake snake;
    snake.reset(20, 20);
    Board board(20, 20);

    for (int i = 0; i < 100; ++i) {
        board.spawn_food(snake);
        auto pos = board.food_position();
        CHECK(pos.x >= 0);
        CHECK(pos.x < 20);
        CHECK(pos.y >= 0);
        CHECK(pos.y < 20);
    }
}

TEST_CASE("Board food does not spawn on snake", "[board]") {
    Snake snake;
    snake.reset(20, 20);
    Board board(20, 20);

    for (int i = 0; i < 100; ++i) {
        board.spawn_food(snake);
        CHECK_FALSE(snake.occupies(board.food_position()));
    }
}

TEST_CASE("Board dimensions match constructor", "[board]") {
    const Board board(15, 25);
    CHECK(board.width() == 15);
    CHECK(board.height() == 25);
}

TEST_CASE("Board bonus food lifecycle", "[board]") {
    Snake snake;
    snake.reset(20, 20);
    Board board(20, 20);

    CHECK_FALSE(board.bonus_position().has_value());

    board.spawn_bonus(snake);
    CHECK(board.bonus_position().has_value());
    CHECK(board.bonus_time_remaining() > 0.f);

    REQUIRE(board.bonus_position().has_value());
    auto pos = *board.bonus_position(); // NOLINT(bugprone-unchecked-optional-access)
    CHECK(pos.x >= 0);
    CHECK(pos.x < 20);
    CHECK(pos.y >= 0);
    CHECK(pos.y < 20);
    CHECK_FALSE(snake.occupies(pos));

    board.update_bonus(10.f);
    CHECK_FALSE(board.bonus_position().has_value());
}

TEST_CASE("Board clear_bonus removes bonus", "[board]") {
    Snake snake;
    snake.reset(20, 20);
    Board board(20, 20);

    board.spawn_bonus(snake);
    CHECK(board.bonus_position().has_value());

    board.clear_bonus();
    CHECK_FALSE(board.bonus_position().has_value());
    CHECK(board.bonus_time_remaining() == 0.f);
}
