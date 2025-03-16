#pragma once

// Current game state
enum class GameState {
    INGAME,         // The game hasn't ended.
    CHECKMATE,      // The current player is in checkmate (has lost).
    STALEMATE,      // The current player is in stalemate (in a draw).
    HALF_MOVE_DRAW, // The game ended due to halfmove draw.
    REPETITION,     // The game ended due to repetition.

    OOB, FIRST = INGAME, LAST = REPETITION
};