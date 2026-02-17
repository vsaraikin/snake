#include "Game.hpp"

#include <cstdlib>
#include <print>

int main() { // NOLINT(bugprone-exception-escape)
    auto game = Game::create();
    if (!game) {
        std::print(stderr, "[snake] Error: {}\n", game.error());
        return EXIT_FAILURE;
    }

    game->run();
    return EXIT_SUCCESS;
}
