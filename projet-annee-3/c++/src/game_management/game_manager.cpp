#include "game_manager.hpp"

#include "../logic/player.hpp"

/* ---- DEFINE class GameManager ---- */

GameManager::GameManager(const std::string& initial_position) : game(initial_position) {}

bool GameManager::makeMove(Move move) {
    return game.move(move);
}

/* ---- END DEFINE ---- */