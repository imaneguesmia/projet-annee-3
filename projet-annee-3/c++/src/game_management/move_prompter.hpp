#pragma once

#include "move_accepter.hpp"

#include "../logic/game_data.hpp"

/* ---- DECLARE class MovePrompter ---- */

/**
 * @brief Service layer class received by a player to propose their chosen moves.
 * 
 * Human players can choose their move using the pygame GUI which receives this class
 * and proposes the chosen move.
 * 
 * Similarly, AI players can choose their move by receiving this class and proposing
 * the move the Search class (or whatever the neural network uses) calculates.
 */
class MovePrompter {
    MoveAccepter& accepter;

    GameData& game_data;
    ExtendedGameData& extended_game_data;

public:
    MovePrompter(MoveAccepter& accepter, ExtendedGameData& game_data)
        : accepter(accepter)
        , game_data(game_data)
        , extended_game_data(game_data)
    {};
    ~MovePrompter() {};

    GameData& gameData() { return game_data; };
    ExtendedGameData& extendedGameData() { return extended_game_data; };

    bool proposeMove(Move move) { return accepter.acceptMove(move); };
};
    
/* ---- END DECLARE ---- */