#include "game_manager.hpp"

#include "../logic/player.hpp"

/* ---- DEFINE class GameManager ---- */

MovePrompter GameManager::promptNextMove() {
    return MovePrompter(*this, game);
}

bool GameManager::acceptMove(Move move) {
    return game.move(move);
}

/* ---- END DEFINE ---- */