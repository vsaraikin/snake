#pragma once

class HighScore {
public:
    void load();
    void save() const;
    [[nodiscard]] bool try_update(int score);
    [[nodiscard]] int value() const { return value_; }

private:
    int value_ = 0;
};
