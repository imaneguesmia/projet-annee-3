#include "game_manager.hpp"

#include "../logic/player.hpp"

/* ---- DEFINE class GameManager ---- */

GameManager::GameManager()
    : at(std::make_shared<const AttackTables>())
    , game(at)
{}

GameManager::GameManager(const std::string& initial_position)
    : at(std::make_shared<const AttackTables>())
    , game(at, initial_position)
{}

bool GameManager::makeMove(Move move) {
    return game.move(move);
}

/* ---- END DEFINE ---- */