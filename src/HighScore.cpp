#include "HighScore.hpp"

#include <fstream>

void HighScore::load() {
    std::ifstream file("highscore.txt");
    int v = 0;
    if (file.is_open() && (file >> v) && v >= 0) {
        value_ = v;
    }
}

void HighScore::save() const {
    std::ofstream file("highscore.txt");
    if (file.is_open()) {
        file << value_;
    }
}

bool HighScore::try_update(int score) {
    if (score > value_) {
        value_ = score;
        save();
        return true;
    }
    return false;
}
