#pragma once

#include "Board.hpp"
#include "Config.hpp"
#include "Snake.hpp"

#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/View.hpp>

#include <expected>
#include <string>

enum class GameState;

struct RenderContext {
    const Snake& snake;
    const Board& board;
    GameState state;
    int score;
    int high_score;
    bool is_new_high_score;
    float alpha; // interpolation factor 0–1
    int cell_size;
    int grid_w;
    int grid_h;
    float elapsed_time; // total elapsed time for animations
    sf::Vector2f shake_offset;
    sf::View game_view;
    // Settings screen
    int settings_cursor;
    bool settings_binding_mode;
    int settings_grid_size;
    std::string settings_speed_label;
    std::string settings_key_up;
    std::string settings_key_down;
    std::string settings_key_left;
    std::string settings_key_right;
    std::string settings_key_pause;
};

class Renderer {
public:
    static std::expected<Renderer, std::string> create();

    void draw(sf::RenderWindow& window, const RenderContext& ctx);

private:
    explicit Renderer(sf::Font font);

    void draw_grid(sf::RenderWindow& window, int grid_w, int grid_h, int cell_size);
    void draw_snake(sf::RenderWindow& window, const Snake& snake, float alpha, int cell_size);
    void draw_food(sf::RenderWindow& window, sf::Vector2i food_pos, int cell_size);
    void draw_bonus_food(sf::RenderWindow& window, sf::Vector2i pos, int cell_size,
                         float elapsed_time, float time_remaining);
    void draw_hud(sf::RenderWindow& window, int score, int high_score);
    void draw_overlay(sf::RenderWindow& window, const sf::View& view, const std::string& title,
                      const std::string& subtitle, const std::string& extra = "");
    void draw_settings(sf::RenderWindow& window, const RenderContext& ctx);

    sf::Font font_;
};
