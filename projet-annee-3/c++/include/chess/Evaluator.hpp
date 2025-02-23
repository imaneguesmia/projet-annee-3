/**
* @file Evaluator.hpp
 * @brief Implements static evaluation using material values and piece-square tables.
 * @link https://www.chessprogramming.org/Evaluation
 *
 * Optimizations used:
 * - Piece-Square Tables
 * - Material evaluation
  * - Pawn Structure
 *
 * To do:
 * - Mobility-based heuristics
 * - King Safety
 * - .....
 */

#pragma once

#include "chess.hpp"

namespace chess {

    /**
     * @class Evaluator
     * @brief Responsible for static board evaluation.
     */
    class Evaluator {
    public:
        /**
         * @brief todo
         */
        Evaluator();

        /**
         * @brief Evaluates the given board position.
         * @param board The current chess board state.
         * @return The evaluation score, positive if the player to move has the advantage,
 *       * negative if the opponent has the advantage.
         */
        int evaluate(const Board& board);

    private:
        /**
         * @brief Mirrors a square index to reflect the board from Black's perspective.
         * @param sq Square index (0-63).
         * @return Mirrored square index.
         */
        int mirrorSquare(int sq) const;

        uint64_t arrNeighborFiles[8];
        const int isolatedScore[8] = { -12, -14, -16, -20, -20, -16, -14, -12 }; // https://beginchess.com/2010/08/15/think-like-a-chess-engine
        const int candidatePassedMidgame[8] = { 0, 6, 6, 14, 34, 83, 0, 0 }; // endgame : const int CandidatePassedEndgame[RANK_NB] = { 0, 13, 13, 29, 68, 166, 0, 0 }; source reddit
        int pawnValues[8] = {-12, -14, -16, -20, -20, -16, -14, -12};     // https://beginchess.com/2010/08/15/think-like-a-chess-engine
        const int BACKWARD = -9;
        const int DOUBLED = -12;

        const int baseValues[8] = {
            /* 0 : PAWN   */  100,
            /* 1 : KNIGHT */  300,
            /* 2 : BISHOP */  300,
            /* 3 : ROOK   */  500,
            /* 4 : QUEEN  */  900,
            /* 5 : KING   */  10000,
            /* 6 : NONE   */  0
        };


        /**
         * @brief Evaluates the pawn structure for a given color and returns a score.
         *
         * This function analyzes isolated, passed, backward, and doubled pawns on the board, 
         * adjusting the score based on their positions and configurations.
         *
         * @param board A reference to the current game board.
         * @param color The color of the pawns being evaluated (WHITE or BLACK).
         *
         * @return int The score for the pawn structure, with positive values favoring the 
         * specified color and negative values indicating disadvantages.
         *
         * @note Scoring is influenced by various pawn structures:
         * - Isolated pawns: Contribute positively based on their file position.
         * - Passed pawns: Score increases with the pawn's rank.
         * - Backward pawns: Penalized based on their count.
         * - Doubled pawns: Each doubled pawn incurs a penalty.
         */
        int pawnStructureHeuristic(const Board& board,Color color);
    };

} // namespace chess

