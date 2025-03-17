/**
 * @file Evaluator.hpp
 * @brief Implements static evaluation using material values and piece-square tables.
 * @link https://www.chessprogramming.org/Evaluation
 *
 * Optimizations used:
 * - Piece-Square Tables
 * - Material evaluation
 * - Pawn Structure
 * - Mobility-based heuristics
 * To do:
 * - King Safety
 * - .....
 */

#pragma once

#include "../logic/bitboard.hpp"
#include "../logic/board.hpp"
#include "../logic/position.hpp"
#include "../logic/player.hpp"
#include "../logic/piece.hpp"
#include "../logic/attack_tables.hpp"

#include <cstdint>
#include <memory>

/**
 * @class Evaluator
 * @brief Responsible for static board evaluation.
 */
class Evaluator
{
    const AttackTables &at = AttackTables::getInstance();

public:
    /**
     * @brief todo
     */
    Evaluator();

    /**
     * @brief Evaluates the given board position.
     * @param board The current chess board state.
     * @param player The player being evaluated (WHITE or BLACK).
     * @return The evaluation score, positive if the player to move has the advantage,
     *       * negative if the opponent has the advantage.
     */
    int evaluate(const Board &board, Player player);

private:
    /**
     * @brief Mirrors a square index to reflect the board from Black's perspective.
     * @param sq Square index (0-63).
     * @return Mirrored square index.
     */
    int mirrorSquare(int sq) const;
    uint64_t arrNeighborFiles[8];
    int baseValues[6];
    const int isolatedScore[8] = {-12, -14, -16, -20, -20, -16, -14, -12}; // https://beginchess.com/2010/08/15/think-like-a-chess-engine
    const int candidatePassedMidgame[8] = {0, 6, 6, 14, 34, 83, 0, 0};     // endgame : const int CandidatePassedEndgame[RANK_NB] = { 0, 13, 13, 29, 68, 166, 0, 0 }; source reddit
    int pawnValues[8] = {-12, -14, -16, -20, -20, -16, -14, -12};          // https://beginchess.com/2010/08/15/think-like-a-chess-engine
    const int BACKWARD = -9;
    const int DOUBLED = -12;
    const int mobility_bonus[5][32] = {
        // MidGame https://tests.stockfishchess.org/tests/view/64a320173ee09aa549c52157
        {},                                                          // To avoid having to do -1
        {-62, -53, -12, -3, 3, 12, 21, 28, 37},                      // Knight
        {-47, -20, 14, 29, 39, 53, 53, 60, 62, 69, 78, 83, 91, 96},  // Bishop
        {-60, -24, 0, 3, 4, 14, 20, 30, 41, 41, 41, 45, 57, 58, 67}, // Rook
        {-29, -16, -8, -8, 18, 25, 23, 37, 41, 54, 65, 68, 69, 70, 70,
         70, 71, 72, 74, 76, 90, 104, 105, 106, 112, 114, 114, 119} // Queen
    };

    /**
     * @brief Evaluates the penalty for isolated pawns on the board.
     *
     * This function calculates a heuristic score based on the number of isolated pawns
     * for the given player. A pawn is considered isolated if there are no friendly pawns
     * on adjacent files.
     * @link https://www.chessprogramming.org/Isolated_Pawn
     * @param board The chess board representation.
     * @param player The player for whom the evaluation is performed.
     * @param pawnBitboard The bitboard representing the positions of the player's pawns.
     * @return int The heuristic score for isolated pawns.
     */
    int isolatedPawnHeuristic(const Board &board, Player player, BB::BitBoard pawnBitboard);
    /**
     * @brief Evaluates the heuristic score for passed pawns on the board.
     *
     * This function calculates a heuristic score based on the number and position
     * of passed pawns for the given player. A passed pawn is a pawn that has no
     * opposing pawns blocking its path to promotion or on adjacent files.
     *
     * @link https://www.chessprogramming.org/Passed_Pawn
     * @param board The chess board representation.
     * @param player The player for whom the evaluation is performed (White or Black).
     * @param pawnBitboard The bitboard representing the positions of the player's pawns.
     * @return int The heuristic score for passed pawns.
     */
    int passedPawnHeuristic(const Board &board, Player player, BB::BitBoard pawnBitboard);
    /**
     * @brief Evaluates the heuristic score for backward pawns on the board.
     *
     * This function calculates a penalty for backward pawns. A backward pawn is one that
     * cannot advance safely due to enemy pawn attacks and lacks support from friendly pawns.
     *
     * @link https://www.chessprogramming.org/Backward_Pawn
     * @param board The chess board representation.
     * @param player The player for whom the evaluation is performed (White or Black).
     * @param pawnBitboard The bitboard representing the positions of the player's pawns.
     * @param oppPawnBitboard The bitboard representing the positions of the opponent's pawns.
     * @return int The heuristic score penalty for backward pawns.
     */
    int backwardPawnHeuristic(const Board &board, Player player, BB::BitBoard pawnBitboard, BB::BitBoard oppPawnBitboard);
    /**
     * @brief Evaluates the heuristic penalty for doubled and tripled pawns.
     *
     * Doubled and tripled pawns occur when two or more pawns of the same player
     * occupy the same file. Such pawns are generally considered weaknesses
     * because they block each other and cannot be supported by adjacent pawns.
     * This function calculates a penalty based on the number of doubled/tripled pawns.
     *
     * @link https://www.chessprogramming.org/Doubled_Pawn
     * @param board The chess board representation.
     * @param player The player for whom the evaluation is performed (White or Black).
     * @param pawnBitboard The bitboard representing the positions of the player's pawns.
     * @return int The heuristic penalty for doubled/tripled pawns.
     */
    int doubleTriplePawnHeuristic(const Board &board, Player player, BB::BitBoard pawnBitboard);
    /**
     * @brief Evaluates the pawn structure for a given color and returns a score.
     *
     * This function analyzes isolated, passed, backward, and doubled pawns on the board,
     * adjusting the score based on their positions and configurations.
     *
     * @param board A reference to the current game board.
     * @param player The playr being evaluated (WHITE or BLACK).
     * @return int The score for the pawn structure, with positive values favoring the
     * specified color and negative values indicating disadvantages.
     *
     * @note Scoring is influenced by various pawn structures:
     * - Isolated pawns: Contribute positively based on their file position.
     * - Passed pawns: Score increases with the pawn's rank.
     * - Backward pawns: Penalized based on their count.
     * - Doubled pawns: Each doubled pawn incurs a penalty.
     */
    int pawnStructureHeuristic(const Board &board, Player player);

    /**
     * @brief Evaluates the mobility heuristic for a given board state and color.
     *
     * This function calculates the mobility score by summing up the number of legal moves
     * available for each piece type (Knight, Bishop, Rook, and Queen) of the given color.
     *
     * @param board The current board state.
     * @param player The player to evaluate.
     * @return The mobility score.
     */

    int mobilityHeuristic(const Board &board, Player player);

    int materialScore(const Board &board, Player player) const;
};
