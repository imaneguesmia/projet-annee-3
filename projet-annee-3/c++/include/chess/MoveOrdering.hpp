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
     * @param playerToMove The player making the move.
     */
    void updateHistory(const Board& board, Move move, int depth, Player playerToMove);

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
     * @param playerToMove The player making the move.
     * @return The history heuristic score.
     */
    int historyScore(const Board& board, Move move, Player playerToMove) const;

    /**
     * @brief Orders moves based on various heuristics (MVV-LVA, killers, history).
     * @param moves List of moves to be ordered.
     * @param board The current board state.
     * @param ply Current search depth.
     * @param pvMove Principal variation move from the transposition table.
     */
//    void orderMoves(Movelist& moves, const Board& board, int ply, Move pvMove);

    /**
     * @brief Assigns a numerical score to each move.
     * @param moves  List of moves to be scored.
     * @param board  Current board state.
     * @param ply    Current search depth.
     * @param pvMove Transposition table's best move, to be heavily rewarded.
     * @param playerToMove The player making the move.
     */
    void scoreMoves(std::vector<Move>& moves, const Board& board, int ply, Move pvMove, Player playerToMove);

    /**
     * @brief Select the best move from [startIndex..end] in 'moves' and swap it to 'startIndex'.
     * @param moves  List of moves already scored with scoreMoves().
     * @param startIndex  The index from which to pick the best move.
     */
    void pickNextMove(std::vector<Move>& moves, int startIndex);

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
    enum_array<Player, enum_array<Square, enum_array<Square, int>>> historyHeuristic; ///< Stores history heuristic values
};