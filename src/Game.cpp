#include "Game.hpp"

#include <SFML/Window/Event.hpp>

#include <algorithm>
#include <array>
#include <print>

std::expected<Game, std::string> Game::create() {
    auto renderer = Renderer::create();
    if (!renderer) {
        return std::unexpected(renderer.error());
    }

    Settings settings;
    settings.load();

    const int win_size = settings.window_size();
    sf::RenderWindow window(
        sf::VideoMode({static_cast<unsigned>(win_size), static_cast<unsigned>(win_size)}),
        Config::window_title, sf::Style::Default);
    window.setFramerateLimit(60);

    std::print("[snake] Window created ({}x{})\n", win_size, win_size);

    Game game{std::move(window), std::move(*renderer)};
    game.settings_ = settings;
    game.board_ = Board(settings.grid_size, settings.grid_size);
    game.board_.spawn_food(game.snake_);

    game.high_score_.load();

    auto view_size = static_cast<float>(win_size);
    game.game_view_ = sf::View(sf::FloatRect({0.f, 0.f}, {view_size, view_size}));
    game.game_start_time_ = Clock::now();

    return game;
}

Game::Game(sf::RenderWindow window, // NOLINT(performance-unnecessary-value-param)
           Renderer renderer)
    : window_(std::move(window)), renderer_(std::move(renderer)),
      board_(Config::grid_width, Config::grid_height), last_tick_(Clock::now()),
      last_frame_time_(Clock::now()), game_start_time_(Clock::now()) {}

void Game::run() {
    std::print("[snake] Game running\n");

    while (window_.isOpen()) {
        auto now = Clock::now();
        const float dt = std::chrono::duration<float>(now - last_frame_time_).count();
        last_frame_time_ = now;

        handle_events();

        if (state_ == GameState::Playing) {
            if (now - last_tick_ >= tick_interval()) {
                last_tick_ = now;
                update();
            }
            board_.update_bonus(dt);
        }

        if (shake_timer_ > 0.f) {
            shake_timer_ -= dt;
            shake_timer_ = std::max(shake_timer_, 0.f);
        }

        float alpha = 0.f;
        if (state_ == GameState::Playing) {
            auto elapsed = std::chrono::duration<float>(now - last_tick_).count();
            const float interval = std::chrono::duration<float>(tick_interval()).count();
            alpha = std::clamp(elapsed / interval, 0.f, 1.f);
        } else {
            alpha = 1.f;
        }

        sf::Vector2f shake_offset{0.f, 0.f};
        if (shake_timer_ > 0.f) {
            const float intensity =
                Config::shake_intensity * (shake_timer_ / Config::shake_duration);
            std::uniform_real_distribution<float> dist(-intensity, intensity);
            shake_offset = {dist(shake_rng_), dist(shake_rng_)};
        }

        const float elapsed_time = std::chrono::duration<float>(now - game_start_time_).count();

        const RenderContext ctx{
            .snake = snake_,
            .board = board_,
            .state = state_,
            .score = score_,
            .high_score = high_score_.value(),
            .is_new_high_score = is_new_high_score_,
            .alpha = alpha,
            .cell_size = Config::cell_size,
            .grid_w = settings_.grid_size,
            .grid_h = settings_.grid_size,
            .elapsed_time = elapsed_time,
            .shake_offset = shake_offset,
            .game_view = game_view_,
            .settings_cursor = settings_cursor_,
            .settings_binding_mode = settings_binding_mode_,
            .settings_grid_size = settings_.grid_size,
            .settings_speed_label = speed_label(),
            .settings_key_up = Settings::key_to_name(settings_.keys.up),
            .settings_key_down = Settings::key_to_name(settings_.keys.down),
            .settings_key_left = Settings::key_to_name(settings_.keys.left),
            .settings_key_right = Settings::key_to_name(settings_.keys.right),
            .settings_key_pause = Settings::key_to_name(settings_.keys.pause),
        };

        renderer_.draw(window_, ctx);
    }
}

void Game::handle_events() {
    while (auto event = window_.pollEvent()) {
        if (event->is<sf::Event::Closed>()) {
            window_.close();
        } else if (const auto* key = event->getIf<sf::Event::KeyPressed>()) {
            handle_key(key->code);
        } else if (const auto* resized = event->getIf<sf::Event::Resized>()) {
            const float window_ratio =
                static_cast<float>(resized->size.x) / static_cast<float>(resized->size.y);
            const float view_ratio = 1.0f;

            sf::FloatRect viewport;
            if (window_ratio > view_ratio) {
                const float width = view_ratio / window_ratio;
                viewport = sf::FloatRect({(1.f - width) / 2.f, 0.f}, {width, 1.f});
            } else {
                const float height = window_ratio / view_ratio;
                viewport = sf::FloatRect({0.f, (1.f - height) / 2.f}, {1.f, height});
            }
            game_view_.setViewport(viewport);
        }
    }
}

void Game::handle_key(sf::Keyboard::Key key) {
    using K = sf::Keyboard::Key;

    if (state_ == GameState::Settings) {
        handle_settings_key(key);
        return;
    }

    switch (state_) {
    case GameState::Menu:
        if (key == K::Enter) {
            start_game();
        } else if (key == K::S) {
            state_ = GameState::Settings;
            settings_cursor_ = 0;
            settings_binding_mode_ = false;
        } else if (key == K::Escape) {
            window_.close();
        }
        break;

    case GameState::Playing:
        if (key == settings_.keys.up || key == K::W)
            snake_.set_direction(Direction::Up); // NOLINT(bugprone-branch-clone)
        else if (key == settings_.keys.down || key == K::S)
            snake_.set_direction(Direction::Down);
        else if (key == settings_.keys.left || key == K::A)
            snake_.set_direction(Direction::Left);
        else if (key == settings_.keys.right || key == K::D)
            snake_.set_direction(Direction::Right);
        else if (key == settings_.keys.pause)
            state_ = GameState::Paused;
        else if (key == K::Escape)
            window_.close();
        break;

    case GameState::Paused:
        if (key == settings_.keys.pause) {
            state_ = GameState::Playing;
            last_tick_ = Clock::now();
        } else if (key == K::Escape) {
            window_.close();
        }
        break;

    case GameState::GameOver:
        if (key == K::Enter) {
            start_game();
        } else if (key == K::Escape) {
            window_.close();
        }
        break;

    default: break;
    }
}

void Game::handle_settings_key(sf::Keyboard::Key key) {
    using K = sf::Keyboard::Key;

    if (settings_binding_mode_) {
        switch (settings_cursor_) {
        case 2: settings_.keys.up = key; break; // NOLINT(bugprone-branch-clone)
        case 3: settings_.keys.down = key; break;
        case 4: settings_.keys.left = key; break;
        case 5: settings_.keys.right = key; break;
        case 6: settings_.keys.pause = key; break;
        default: break;
        }
        settings_binding_mode_ = false;
        return;
    }

    switch (key) {
    case K::Up:
        settings_cursor_ = (settings_cursor_ - 1 + settings_item_count) % settings_item_count;
        break;
    case K::Down: settings_cursor_ = (settings_cursor_ + 1) % settings_item_count; break;
    case K::Left:
        if (settings_cursor_ == 0)
            cycle_grid_size(-1);
        else if (settings_cursor_ == 1)
            cycle_speed(-1);
        break;
    case K::Right:
        if (settings_cursor_ == 0)
            cycle_grid_size(1);
        else if (settings_cursor_ == 1)
            cycle_speed(1);
        break;
    case K::Enter:
        if (settings_cursor_ >= 2 && settings_cursor_ <= 6) {
            settings_binding_mode_ = true;
        } else if (settings_cursor_ == 7) {
            settings_.save();
            apply_settings_changes();
            state_ = GameState::Menu;
        }
        break;
    case K::Escape:
        settings_.save();
        apply_settings_changes();
        state_ = GameState::Menu;
        break;
    default: break;
    }
}

void Game::update() {
    snake_.update();

    if (snake_.is_out_of_bounds(settings_.grid_size, settings_.grid_size) ||
        snake_.has_self_collision()) {
        state_ = GameState::GameOver;
        is_new_high_score_ = high_score_.try_update(score_);
        shake_timer_ = Config::shake_duration;
        std::print("[snake] Game over! Final score: {}\n", score_);
        return;
    }

    if (snake_.head() == board_.food_position()) {
        snake_.grow();
        ++score_;
        board_.spawn_food(snake_);
        std::print("[snake] Score: {}\n", score_);

        std::uniform_real_distribution<float> chance(0.f, 1.f);
        if (!board_.bonus_position() && chance(shake_rng_) < Config::bonus_spawn_chance) {
            board_.spawn_bonus(snake_);
        }
    }

    if (board_.bonus_position() && snake_.head() == *board_.bonus_position()) {
        score_ += Config::bonus_points;
        board_.clear_bonus();
        std::print("[snake] Bonus! Score: {}\n", score_);
    }
}

void Game::start_game() {
    snake_.reset(settings_.grid_size, settings_.grid_size);
    board_ = Board(settings_.grid_size, settings_.grid_size);
    board_.spawn_food(snake_);
    score_ = 0;
    is_new_high_score_ = false;
    state_ = GameState::Playing;
    last_tick_ = Clock::now();
    std::print("[snake] New game started\n");
}

std::chrono::milliseconds Game::tick_interval() const {
    const int speedups = score_ / Config::speed_increment_score;
    auto ms = settings_.starting_speed - std::chrono::milliseconds(speedups * 10);
    return std::max(ms, Config::min_tick);
}

void Game::cycle_grid_size(int dir) {
    static constexpr std::array sizes = {15, 20, 25, 30};
    int idx = 0;
    for (int i = 0; i < 4; ++i) {
        if (sizes[i] == settings_.grid_size) {
            idx = i;
            break;
        }
    }
    idx = (idx + dir + 4) % 4;
    settings_.grid_size = sizes[idx];
}

void Game::cycle_speed(int dir) {
    static constexpr std::array speeds = {Settings::speed_slow, Settings::speed_medium,
                                          Settings::speed_fast};
    const int current = static_cast<int>(settings_.starting_speed.count());
    int idx = 0;
    for (int i = 0; i < 3; ++i) {
        if (speeds[i] == current) {
            idx = i;
            break;
        }
    }
    idx = (idx + dir + 3) % 3;
    settings_.starting_speed = std::chrono::milliseconds(speeds[idx]);
}

std::string Game::speed_label() const {
    const int ms = static_cast<int>(settings_.starting_speed.count());
    if (ms == Settings::speed_slow) return "Slow";
    if (ms == Settings::speed_medium) return "Medium";
    if (ms == Settings::speed_fast) return "Fast";
    return "Custom";
}

void Game::apply_settings_changes() {
    const int win_size = settings_.window_size();
    window_.setSize({static_cast<unsigned>(win_size), static_cast<unsigned>(win_size)});
    auto view_size = static_cast<float>(win_size);
    game_view_ = sf::View(sf::FloatRect({0.f, 0.f}, {view_size, view_size}));
    game_view_.setViewport(sf::FloatRect({0.f, 0.f}, {1.f, 1.f}));

    board_ = Board(settings_.grid_size, settings_.grid_size);
    snake_.reset(settings_.grid_size, settings_.grid_size);
    board_.spawn_food(snake_);

    std::print("[snake] Settings applied: grid={}, speed={}ms\n", settings_.grid_size,
               settings_.starting_speed.count());
}
