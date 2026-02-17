#pragma once

#include "Board.hpp"
#include "HighScore.hpp"
#include "Renderer.hpp"
#include "Settings.hpp"
#include "Snake.hpp"

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/View.hpp>

#include <chrono>
#include <expected>
#include <random>
#include <string>

enum class GameState { Menu, Playing, Paused, GameOver, Settings };

class Game {
public:
    static std::expected<Game, std::string> create();
    void run();

private:
    Game(sf::RenderWindow window, Renderer renderer);

    void handle_events();
    void handle_key(sf::Keyboard::Key key);
    void handle_settings_key(sf::Keyboard::Key key);
    void update();
    void start_game();
    void apply_settings_changes();

    [[nodiscard]] std::chrono::milliseconds tick_interval() const;

    // Settings screen helpers
    void cycle_grid_size(int dir);
    void cycle_speed(int dir);
    [[nodiscard]] std::string speed_label() const;

    sf::RenderWindow window_;
    Renderer renderer_;
    Snake snake_;
    Board board_;
    Settings settings_;
    HighScore high_score_;

    GameState state_ = GameState::Menu;
    int score_ = 0;
    bool is_new_high_score_ = false;

    // Timing
    using Clock = std::chrono::steady_clock;
    Clock::time_point last_tick_;
    Clock::time_point last_frame_time_;
    Clock::time_point game_start_time_;

    // Screen shake
    float shake_timer_ = 0.f;
    std::mt19937 shake_rng_{std::random_device{}()};

    // View for letterboxing
    sf::View game_view_;

    // Settings screen state
    int settings_cursor_ = 0;
    bool settings_binding_mode_ = false;
    static constexpr int settings_item_count = 8;
};
