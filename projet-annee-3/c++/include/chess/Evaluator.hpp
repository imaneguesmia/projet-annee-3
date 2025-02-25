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
 *
 * To do:
 * - King Safety
 * - .....
 */

#pragma once
#include "chess/utils.hpp"
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

        // Pawn structure
        uint64_t neighbor_files_bb[8];
        const int isolatedScore[8] = { -12, -14, -16, -20, -20, -16, -14, -12 }; // https://beginchess.com/2010/08/15/think-like-a-chess-engine
        const int candidate_passed_midgame[8] = { 0, 6, 6, 14, 34, 83, 0, 0 }; // endgame : const int CandidatePassedEndgame[RANK_NB] = { 0, 13, 13, 29, 68, 166, 0, 0 }; source reddit
        int pawn_values[8] = {-12, -14, -16, -20, -20, -16, -14, -12};     // https://beginchess.com/2010/08/15/think-like-a-chess-engine
        const int BACKWARD = -9;
        const int DOUBLED = -12;

        const int mobility_bonus[5][32] = { //MidGame https://tests.stockfishchess.org/tests/view/64a320173ee09aa549c52157
            {}, // To avoid having to do -1
            { -62, -53, -12,  -3,   3,  12,  21,  28,  37 },  // Knight
            { -47, -20,  14,  29,  39,  53,  53,  60,  62,  69,  78,  83,  91,  96 },  // Bishop
            { -60, -24,   0,   3,   4,  14,  20,  30,  41,  41,  41,  45,  57,  58,  67 },  // Rook
            { -29, -16,  -8,  -8,  18,  25,  23,  37,  41,  54,  65,  68,  69,  70,  70,  
            70,  71,  72,  74,  76,  90, 104, 105, 106, 112, 114, 114, 119 }  // Queen
        };

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

    /**
     * @brief Evaluates the mobility heuristic for a given board state and color.
     *
     * This function calculates the mobility score by summing up the number of legal moves
     * available for each piece type (Knight, Bishop, Rook, and Queen) of the given color.
     *
     * @param board The current board state.
     * @param color The color of the pieces to evaluate.
     * @return The mobility score.
     */
    
    int mobilityHeuristic(const Board& board, Color color);
};

} // namespace chess

