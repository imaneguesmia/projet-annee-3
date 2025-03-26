#include "game_manager.hpp"

#include "../logic/player.hpp"

/* ---- DEFINE class GameManager ---- */

GameManager::GameManager(const std::string& initial_position) : game(initial_position) {}

bool GameManager::makeMove(Move move) {
    return game.move(move);
}

bool GameManager::makeMove(const Position& from, const Position& to, const PType promoted_to) {
    return game.move(from, to, promoted_to);
}

void GameManager::undoLastMove() {
    game.undoLastMove();
}

/* ---- END DEFINE ---- */