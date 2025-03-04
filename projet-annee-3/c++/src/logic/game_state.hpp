#pragma once

// Current game state
enum class GameState {
    INGAME,         // The game hasn't ended.
    CHECKMATE,      // The current player is in checkmate (has lost).
    STALEMATE,      // The current player is in stalemate (in a draw).

    OOB, FIRST = INGAME, LAST = STALEMATE
};