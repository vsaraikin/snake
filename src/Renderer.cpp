#include "Renderer.hpp"

#include "Game.hpp"

#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/Text.hpp>
#include <SFML/Graphics/VertexArray.hpp>

#include <cmath>

std::expected<Renderer, std::string> Renderer::create() {
    sf::Font font; // NOLINT(misc-const-correctness)
    if (!font.openFromFile(Config::font_path)) {
        return std::unexpected("Failed to load font: " + std::string(Config::font_path));
    }
    return Renderer{std::move(font)};
}

Renderer::Renderer(sf::Font font) : font_(std::move(font)) {}

void Renderer::draw(sf::RenderWindow& window, const RenderContext& ctx) {
    window.clear(Config::background);

    sf::View shaken_view = ctx.game_view;
    shaken_view.setCenter(shaken_view.getCenter() + ctx.shake_offset);
    window.setView(shaken_view);

    draw_grid(window, ctx.grid_w, ctx.grid_h, ctx.cell_size);
    draw_food(window, ctx.board.food_position(), ctx.cell_size);

    if (auto bonus = ctx.board.bonus_position()) {
        draw_bonus_food(window, *bonus, ctx.cell_size, ctx.elapsed_time,
                        ctx.board.bonus_time_remaining());
    }

    draw_snake(window, ctx.snake, ctx.alpha, ctx.cell_size);

    window.setView(ctx.game_view);

    switch (ctx.state) {
    case GameState::Menu: {
        const std::string sub = "Press Enter to Start  |  S for Settings";
        const std::string extra = "High Score: " + std::to_string(ctx.high_score);
        draw_overlay(window, ctx.game_view, "SNAKE", sub, extra);
        break;
    }
    case GameState::Playing: // NOLINT(bugprone-branch-clone)
        draw_hud(window, ctx.score, ctx.high_score);
        break;
    case GameState::Paused:
        draw_hud(window, ctx.score, ctx.high_score);
        draw_overlay(window, ctx.game_view, "PAUSED", "Press P to Resume");
        break;
    case GameState::GameOver: {
        const std::string sub = "Score: " + std::to_string(ctx.score) +
                                "  |  Best: " + std::to_string(ctx.high_score) +
                                "  |  Enter to Restart";
        std::string extra;
        if (ctx.is_new_high_score) {
            const float pulse = (std::sin(ctx.elapsed_time * 6.0f) + 1.0f) / 2.0f;
            if (pulse > 0.5f) {
                extra = "NEW HIGH SCORE!";
            }
        }
        draw_overlay(window, ctx.game_view, "GAME OVER", sub, extra);
        break;
    }
    case GameState::Settings: draw_settings(window, ctx); break;
    }

    window.display();
}

void Renderer::draw_grid(sf::RenderWindow& window, int grid_w, int grid_h, int cell_size) {
    auto w = static_cast<float>(grid_w * cell_size);
    auto h = static_cast<float>(grid_h * cell_size);
    const auto cs = static_cast<float>(cell_size);

    sf::VertexArray lines(sf::PrimitiveType::Lines);

    for (int i = 1; i < grid_w; ++i) {
        float x = static_cast<float>(i) * cs;
        lines.append(sf::Vertex{{x, 0.f}, Config::grid_line});
        lines.append(sf::Vertex{{x, h}, Config::grid_line});
    }
    for (int i = 1; i < grid_h; ++i) {
        float y = static_cast<float>(i) * cs;
        lines.append(sf::Vertex{{0.f, y}, Config::grid_line});
        lines.append(sf::Vertex{{w, y}, Config::grid_line});
    }

    window.draw(lines);
}

void Renderer::draw_snake(sf::RenderWindow& window, const Snake& snake, float alpha,
                          int cell_size) {
    const auto cs = static_cast<float>(cell_size);
    const float padding = 1.f;

    const auto& body = snake.body();
    const auto& prev = snake.prev_body();

    for (std::size_t i = 0; i < body.size(); ++i) {
        const sf::Vector2f current = Board::grid_to_pixel(body[i], cell_size);

        sf::Vector2f pos;
        if (i < prev.size()) {
            const sf::Vector2f previous = Board::grid_to_pixel(prev[i], cell_size);
            pos = previous + alpha * (current - previous);
        } else {
            pos = current;
        }

        sf::RectangleShape rect({cs - padding * 2.f, cs - padding * 2.f});
        rect.setPosition(pos + sf::Vector2f{padding, padding});

        const bool is_head = (i == 0);
        rect.setFillColor(is_head ? Config::snake_head : Config::snake_body);

        if (is_head) {
            rect.setOutlineThickness(-2.f);
            rect.setOutlineColor(
                sf::Color(Config::snake_head.r, Config::snake_head.g, Config::snake_head.b, 120));
        }

        window.draw(rect);
    }
}

void Renderer::draw_food(sf::RenderWindow& window, sf::Vector2i food_pos, int cell_size) {
    const auto cs = static_cast<float>(cell_size);
    const float radius = cs / 2.f - 2.f;

    sf::CircleShape circle(radius);
    auto pixel = Board::grid_to_pixel(food_pos, cell_size);
    circle.setPosition(pixel + sf::Vector2f{cs / 2.f - radius, cs / 2.f - radius});
    circle.setFillColor(Config::food_color);

    window.draw(circle);
}

void Renderer::draw_bonus_food(sf::RenderWindow& window, sf::Vector2i pos, int cell_size,
                               float elapsed_time, float time_remaining) {
    const auto cs = static_cast<float>(cell_size);
    const float pulse = 1.0f + 0.15f * std::sin(elapsed_time * 8.0f);
    const float base_radius = cs / 2.f - 2.f;
    const float radius = base_radius * pulse;

    const float alpha_val = (time_remaining < 1.0f) ? time_remaining : 1.0f;

    sf::CircleShape circle(radius);
    auto pixel = Board::grid_to_pixel(pos, cell_size);
    circle.setPosition(pixel + sf::Vector2f{cs / 2.f - radius, cs / 2.f - radius});
    circle.setFillColor(sf::Color(Config::bonus_food_color.r, Config::bonus_food_color.g,
                                  Config::bonus_food_color.b,
                                  static_cast<std::uint8_t>(255.f * alpha_val)));

    window.draw(circle);
}

void Renderer::draw_hud(sf::RenderWindow& window, int score, int high_score) {
    sf::Text text(
        font_, "Score: " + std::to_string(score) + "  |  Best: " + std::to_string(high_score), 20);
    text.setFillColor(Config::text_color);
    text.setPosition({10.f, 5.f});
    window.draw(text);
}

void Renderer::draw_overlay(sf::RenderWindow& window, const sf::View& view,
                            const std::string& title, const std::string& subtitle,
                            const std::string& extra) {
    const auto view_size = view.getSize();
    const float w = view_size.x;
    const float h = view_size.y;

    sf::RectangleShape overlay({w, h});
    overlay.setFillColor(Config::overlay_bg);
    window.draw(overlay);

    sf::Text title_text(font_, title, 48);
    title_text.setFillColor(Config::text_color);
    auto title_bounds = title_text.getLocalBounds();
    title_text.setOrigin({title_bounds.position.x + title_bounds.size.x / 2.f,
                          title_bounds.position.y + title_bounds.size.y / 2.f});
    title_text.setPosition({w / 2.f, h / 2.f - 30.f});
    window.draw(title_text);

    sf::Text sub_text(font_, subtitle, 20);
    sub_text.setFillColor(
        sf::Color(Config::text_color.r, Config::text_color.g, Config::text_color.b, 180));
    auto sub_bounds = sub_text.getLocalBounds();
    sub_text.setOrigin({sub_bounds.position.x + sub_bounds.size.x / 2.f,
                        sub_bounds.position.y + sub_bounds.size.y / 2.f});
    sub_text.setPosition({w / 2.f, h / 2.f + 30.f});
    window.draw(sub_text);

    if (!extra.empty()) {
        sf::Text extra_text(font_, extra, 24);
        extra_text.setFillColor(Config::bonus_food_color);
        auto extra_bounds = extra_text.getLocalBounds();
        extra_text.setOrigin({extra_bounds.position.x + extra_bounds.size.x / 2.f,
                              extra_bounds.position.y + extra_bounds.size.y / 2.f});
        extra_text.setPosition({w / 2.f, h / 2.f + 70.f});
        window.draw(extra_text);
    }
}

void Renderer::draw_settings(sf::RenderWindow& window, const RenderContext& ctx) {
    const auto view_size = ctx.game_view.getSize();
    const float w = view_size.x;
    const float h = view_size.y;

    sf::RectangleShape bg({w, h});
    bg.setFillColor(Config::background);
    window.draw(bg);

    sf::Text title(font_, "SETTINGS", 36);
    title.setFillColor(Config::text_color);
    auto tb = title.getLocalBounds();
    title.setOrigin({tb.position.x + tb.size.x / 2.f, tb.position.y + tb.size.y / 2.f});
    title.setPosition({w / 2.f, 50.f});
    window.draw(title);

    struct Item {
        std::string label;
        std::string value;
    };

    std::vector<Item> items = {
        {"Grid Size", "< " + std::to_string(ctx.settings_grid_size) + " >"},
        {"Speed", "< " + ctx.settings_speed_label + " >"},
        {"Up", "[" + ctx.settings_key_up + "]"},
        {"Down", "[" + ctx.settings_key_down + "]"},
        {"Left", "[" + ctx.settings_key_left + "]"},
        {"Right", "[" + ctx.settings_key_right + "]"},
        {"Pause", "[" + ctx.settings_key_pause + "]"},
        {"Back", ""},
    };

    const float y_start = 110.f;
    const float y_step = 40.f;

    for (int i = 0; i < static_cast<int>(items.size()); ++i) {
        const bool selected = (i == ctx.settings_cursor);
        sf::Color color = // NOLINT(misc-const-correctness)
            selected ? Config::snake_head : Config::text_color;

        if (ctx.settings_binding_mode && selected && i >= 2 && i <= 6) {
            color = Config::bonus_food_color;
        }

        std::string display = items[i].label;
        if (!items[i].value.empty()) {
            display += ":  " + items[i].value;
        }

        if (ctx.settings_binding_mode && selected) {
            display = items[i].label + ":  Press any key...";
        }

        const std::string prefix = selected ? "> " : "  ";

        sf::Text text(font_, prefix + display, 22);
        text.setFillColor(color);
        text.setPosition({w * 0.2f, y_start + static_cast<float>(i) * y_step});
        window.draw(text);
    }
}
