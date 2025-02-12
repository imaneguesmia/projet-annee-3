/**
* @file MoveOrdering.hpp
 * @brief Implements move ordering heuristics for efficient alpha-beta pruning.
 * @link https://www.chessprogramming.org/Move_Ordering
 *
 * Optimizations used:
 * - MVV-LVA (Most Valuable Victim - Least Valuable Attacker)
 * - Killer Moves heuristic
 * - History Heuristic
 * - TT move ordering(Hash/PV Move)
 *
 * To do:
 * - SEE
 * - Late Move Reduction (LMR)
 * - ....
 */

#pragma once

#include <array>
#include <vector>
#include "chess.hpp"

namespace chess {

/**
 * @class MoveOrdering
 * @brief Implements move ordering heuristics for efficient alpha-beta pruning.
 */
class MoveOrdering
{
public:
    /**
     * @brief Constructs the MoveOrdering object.
     * Initializes killer moves and history heuristic tables.
     */
    MoveOrdering();

    /**
     * @brief Updates the killer move table.
     * @param move The move to be stored as a killer move.
     * @param ply The current search depth.
     */
    void updateKillers(Move move, int ply);

    /**
     * @brief Updates the history heuristic table.
     * @param board The current board state.
     * @param move The move being played.
     * @param depth Search depth at which the move was played.
     */
    void updateHistory(const Board& board, Move move, int depth);

    /**
     * @brief Retrieves a killer move from the table.
     * @param ply Search depth.
     * @param index Index (0 or 1) of the killer move.
     * @return The stored killer move.
     */
    Move killerAt(int ply, int index) const;

    /**
     * @brief Retrieves the history heuristic score for a move.
     * @param board The current board state.
     * @param move The move to evaluate.
     * @return The history heuristic score.
     */
    int historyScore(const Board& board, Move move) const;

    /**
     * @brief Orders moves based on various heuristics (MVV-LVA, killers, history).
     * @param moves List of moves to be ordered.
     * @param board The current board state.
     * @param ply Current search depth.
     * @param pvMove Principal variation move from the transposition table.
     */
    void orderMoves(Movelist& moves, const Board& board, int ply, Move pvMove);

    /**
     * @brief Reduces all history heuristic values over time.
     * Prevents old moves from having excessive influence over move ordering.
     *
     * This function could be called:
     * - **After each search iteration**
     * - **Before making a new move** in the game loop
     * - **At intervals during search** (e.g., every 4 plies in `negamax()`)
     */
    void decayHistory();

private:
    static constexpr int MAX_PLY = 64; ///< Maximum search depth for move ordering.
    static constexpr int HISTORY_MAX = 32768; ///< Capping history values

    Move killerMoves[MAX_PLY][2]; ///< Stores killer moves for each ply (max 2 per ply).
    int historyHeuristic[2][64][64] = {}; ///< Stores history heuristic values

};

} // namespace chess

