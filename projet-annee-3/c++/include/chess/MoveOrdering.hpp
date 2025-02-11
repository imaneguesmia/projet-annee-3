/**
* @file MoveOrdering.hpp
 * @brief Implements move ordering heuristics for efficient alpha-beta pruning.
 * @link https://www.chessprogramming.org/Move_Ordering
 *
 * Optimizations used:
 * - MVV-LVA (Most Valuable Victim - Least Valuable Attacker)
 * - Killer Moves heuristic
 * - History Heuristic
 *
 * To do:
 * - SEE
 * - Late Move Reduction (LMR)
 * - Hash Move
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

private:
    std::vector<std::array<Move, 2>> killerMoves; ///< Stores killer moves for each ply (max 2 per ply).
    std::vector<std::vector<int>> historyHeuristic; ///< Stores history heuristic values (16 piece types × 64 squares).

    static constexpr int MAX_PLY = 64; ///< Maximum search depth for move ordering.
};

} // namespace chess

