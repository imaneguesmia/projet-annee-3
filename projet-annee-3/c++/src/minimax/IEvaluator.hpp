#pragma once

#include "../logic/board.hpp"
#include "../logic/player.hpp"

/**
 * @class IEvaluator
 * @brief Interface for board evaluation strategies.
 */
class IEvaluator {
public:
    /**
     * @brief Virtual destructor for proper cleanup in derived classes.
     */
    virtual ~IEvaluator() = default;
    
    /**
     * @brief Evaluates the given board position.
     * @param board The current chess board state.
     * @param player The player being evaluated (WHITE or BLACK).
     * @return The evaluation score, positive if the player to move has the advantage,
     *         negative if the opponent has the advantage.
     */
    virtual int evaluate(const Board& board, Player player) = 0;
}; 