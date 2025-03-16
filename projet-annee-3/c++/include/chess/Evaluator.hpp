/**
* @file Evaluator.hpp
 * @brief Implements static evaluation using material values and piece-square tables.
 * @link https://www.chessprogramming.org/Evaluation
 *
 * Optimizations used:
 * - Piece-Square Tables
 * - Material evaluation
 *
 * To do:
 * - Pawn Structure
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
         * @brief Default constructor for the evaluator.
         */
        Evaluator() = default;

        /**
         * @brief Evaluates the given board position.
         * @param board The current chess board state.
         * @return The evaluation score, positive if the player to move has the advantage,
 *       * negative if the opponent has the advantage.
         */
        int evaluate(const Board& board);
        
        /**
         * @brief Gets the material value of a piece.
         * @param piece The chess piece.
         * @return The material value of the piece.
         */
        int getPieceValue(Piece piece) const;

    private:
        /**
         * @brief Mirrors a square index to reflect the board from Black's perspective.
         * @param sq Square index (0-63).
         * @return Mirrored square index.
         */
        int mirrorSquare(int sq) const;
        
        // Material values for pieces
        static constexpr int PAWN_VALUE = 100;
        static constexpr int KNIGHT_VALUE = 320;
        static constexpr int BISHOP_VALUE = 330;
        static constexpr int ROOK_VALUE = 500;
        static constexpr int QUEEN_VALUE = 900;
    };

} // namespace chess

