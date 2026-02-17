#include "Settings.hpp"

#include <exception>
#include <fstream>
#include <unordered_map>

namespace {

const std::unordered_map<std::string, sf::Keyboard::Key>& name_to_key_map() {
    static const std::unordered_map<std::string, sf::Keyboard::Key> map = {
        {"A", sf::Keyboard::Key::A},           {"B", sf::Keyboard::Key::B},
        {"C", sf::Keyboard::Key::C},           {"D", sf::Keyboard::Key::D},
        {"E", sf::Keyboard::Key::E},           {"F", sf::Keyboard::Key::F},
        {"G", sf::Keyboard::Key::G},           {"H", sf::Keyboard::Key::H},
        {"I", sf::Keyboard::Key::I},           {"J", sf::Keyboard::Key::J},
        {"K", sf::Keyboard::Key::K},           {"L", sf::Keyboard::Key::L},
        {"M", sf::Keyboard::Key::M},           {"N", sf::Keyboard::Key::N},
        {"O", sf::Keyboard::Key::O},           {"P", sf::Keyboard::Key::P},
        {"Q", sf::Keyboard::Key::Q},           {"R", sf::Keyboard::Key::R},
        {"S", sf::Keyboard::Key::S},           {"T", sf::Keyboard::Key::T},
        {"U", sf::Keyboard::Key::U},           {"V", sf::Keyboard::Key::V},
        {"W", sf::Keyboard::Key::W},           {"X", sf::Keyboard::Key::X},
        {"Y", sf::Keyboard::Key::Y},           {"Z", sf::Keyboard::Key::Z},
        {"Up", sf::Keyboard::Key::Up},         {"Down", sf::Keyboard::Key::Down},
        {"Left", sf::Keyboard::Key::Left},     {"Right", sf::Keyboard::Key::Right},
        {"Space", sf::Keyboard::Key::Space},   {"Enter", sf::Keyboard::Key::Enter},
        {"Escape", sf::Keyboard::Key::Escape}, {"Tab", sf::Keyboard::Key::Tab},
    };
    return map;
}

const std::unordered_map<sf::Keyboard::Key, std::string>& key_to_name_map() {
    static std::unordered_map<sf::Keyboard::Key, std::string> map;
    if (map.empty()) {
        for (const auto& [name, key] : name_to_key_map()) {
            map[key] = name;
        }
    }
    return map;
}

} // namespace

std::string Settings::key_to_name(sf::Keyboard::Key key) {
    const auto& map = key_to_name_map();
    auto it = map.find(key);
    return it != map.end() ? it->second : "Unknown";
}

sf::Keyboard::Key Settings::name_to_key(const std::string& name) {
    const auto& map = name_to_key_map();
    auto it = map.find(name);
    return it != map.end() ? it->second : sf::Keyboard::Key::Unknown;
}

void Settings::load() {
    std::ifstream file("settings.txt");
    if (!file.is_open()) return;

    std::string line;
    while (std::getline(file, line)) {
        auto eq = line.find('=');
        if (eq == std::string::npos) continue;

        const std::string key = line.substr(0, eq);
        const std::string val = line.substr(eq + 1);

        try {
            if (key == "grid_size") {
                const int v = std::stoi(val);
                if (v == 15 || v == 20 || v == 25 || v == 30) grid_size = v;
            } else if (key == "speed") {
                const int v = std::stoi(val);
                if (v == speed_slow || v == speed_medium || v == speed_fast)
                    starting_speed = std::chrono::milliseconds(v);
            }
        } catch (const std::exception&) {
            continue;
        }

        if (key == "key_up") { // NOLINT(bugprone-branch-clone)
            keys.up = name_to_key(val);
        } else if (key == "key_down") {
            keys.down = name_to_key(val);
        } else if (key == "key_left") {
            keys.left = name_to_key(val);
        } else if (key == "key_right") {
            keys.right = name_to_key(val);
        } else if (key == "key_pause") {
            keys.pause = name_to_key(val);
        }
    }
}

void Settings::save() const {
    std::ofstream file("settings.txt");
    if (!file.is_open()) return;

    file << "grid_size=" << grid_size << "\n";
    file << "speed=" << starting_speed.count() << "\n";
    file << "key_up=" << key_to_name(keys.up) << "\n";
    file << "key_down=" << key_to_name(keys.down) << "\n";
    file << "key_left=" << key_to_name(keys.left) << "\n";
    file << "key_right=" << key_to_name(keys.right) << "\n";
    file << "key_pause=" << key_to_name(keys.pause) << "\n";
}
