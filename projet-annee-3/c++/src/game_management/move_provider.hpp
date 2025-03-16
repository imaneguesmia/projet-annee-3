#pragma once

#include "../logic/player.hpp"

class Move;
class ExtendedGameData;

/**
 * @brief Interface for AI move providers. Exposes a method to get the move chosen by the AI.
 */
class AIMoveProvider {

public:
    virtual ~AIMoveProvider() {};

    virtual Move getMove(ExtendedGameData& board) = 0;
    virtual int getPositionValue(ExtendedGameData& board, Player player) = 0;
};