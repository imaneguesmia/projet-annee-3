#include "chess/Evaluator.hpp"
#include "chess.hpp"
#include <array>

namespace chess {

// Base material values for each piece type
static constexpr int baseValues[] = {
    100,   // Pawn
    300,   // Knight
    300,   // Bishop
    500,   // Rook
    900,   // Queen
    10000, // King
    0      // None (empty square)
};

// Piece-square tables for positional evaluation
static constexpr int pieceSquareTable[7][64] =
{
    // Pawn
    {
        0, 0, 0, 0, 0, 0, 0, 0,
        50,50,50,50,50,50,50,50,
        10,10,20,30,30,20,10,10,
        5, 5,10,25,25,10, 5, 5,
        0, 0, 0,20,20, 0, 0, 0,
        5,-5,-10, 0, 0,-10,-5, 5,
        5,10,10,-20,-20,10,10, 5,
        0, 0, 0, 0, 0, 0, 0, 0
    },
    // Knight
    {
       -50,-40,-30,-30,-30,-30,-40,-50,
       -40,-20,  0,  0,  0,  0,-20,-40,
       -30,  0, 10, 15, 15, 10,  0,-30,
       -30,  5, 15, 20, 20, 15,  5,-30,
       -30,  0, 15, 20, 20, 15,  0,-30,
       -30,  5, 10, 15, 15, 10,  5,-30,
       -40,-20,  0,  5,  5,  0,-20,-40,
       -50,-40,-30,-30,-30,-30,-40,-50
    },
    // Bishop
    {
       -20,-10,-10,-10,-10,-10,-10,-20,
       -10,  0,  0,  0,  0,  0,  0,-10,
       -10,  0,  5, 10, 10,  5,  0,-10,
       -10,  5,  5, 10, 10,  5,  5,-10,
       -10,  0, 10, 10, 10, 10,  0,-10,
       -10, 10, 10, 10, 10, 10, 10,-10,
       -10,  5,  0,  0,  0,  0,  5,-10,
       -20,-10,-10,-10,-10,-10,-10,-20
    },
    // Rook
    {
         0,  0,  0,  0,  0,  0,  0,  0,
         5, 10, 10, 10, 10, 10, 10,  5,
        -5,  0,  0,  0,  0,  0,  0, -5,
        -5,  0,  0,  0,  0,  0,  0, -5,
        -5,  0,  0,  0,  0,  0,  0, -5,
        -5,  0,  0,  0,  0,  0,  0, -5,
        -5,  0,  0,  0,  0,  0,  0, -5,
         0,  0,  0,  5,  5,  0,  0,  0
    },
    // Queen
    {
        -20,-10,-10, -5, -5,-10,-10,-20,
        -10,  0,  0,  0,  0,  0,  0,-10,
        -10,  0,  5,  5,  5,  5,  0,-10,
         -5,  0,  5,  5,  5,  5,  0, -5,
          0,  0,  5,  5,  5,  5,  0, -5,
        -10,  5,  5,  5,  5,  5,  0,-10,
        -10,  0,  5,  0,  0,  0,  0,-10,
        -20,-10,-10, -5, -5,-10,-10,-20
    },
    // King midgame
    {
        -30,-40,-40,-50,-50,-40,-40,-30,
        -30,-40,-40,-50,-50,-40,-40,-30,
        -30,-40,-40,-50,-50,-40,-40,-30,
        -30,-40,-40,-50,-50,-40,-40,-30,
        -20,-30,-30,-40,-40,-30,-30,-20,
        -10,-20,-20,-20,-20,-20,-20,-10,
        20, 20,  0,  0,  0,  0, 20, 20,
        20, 30, 10,  0,  0, 10, 30, 20
    },
    // King endgame
    {
        -50,-40,-30,-20,-20,-30,-40,-50,
        -30,-20,-10,  0,  0,-10,-20,-30,
        -30,-10, 20, 30, 30, 20,-10,-30,
        -30,-10, 30, 40, 40, 30,-10,-30,
        -30,-10, 30, 40, 40, 30,-10,-30,
        -30,-10, 20, 30, 30, 20,-10,-30,
        -30,-30,  0,  0,  0,  0,-30,-30,
        -50,-30,-30,-30,-30,-30,-30,-50
    }
};

/**
 * @brief Mirrors a square index to reflect the board from White's perspective.
 * @param sq Square index (0-63).
 * @return Mirrored square index.
 */
int Evaluator::mirrorSquare(int sq) const
{
    return sq ^ 56; // Flips the rank for Black's perspective
}

/**
 * @brief Gets the material value of a piece.
 * @param piece The chess piece.
 * @return The material value of the piece.
 */
int Evaluator::getPieceValue(Piece piece) const
{
    if (piece == Piece::NONE) {
        return 0;
    }
    
    PieceType pt = piece.type();
    return baseValues[static_cast<int>(pt)];
}

/**
 * @brief Evaluates the given board position.
 * @param board The current chess board state.
 * @return The evaluation score from the current's Player perspective.
 */
int Evaluator::evaluate(const Board& board)
{
    int score = 0;

    for (auto pt : {
        PieceType::PAWN,
        PieceType::KNIGHT,
        PieceType::BISHOP,
        PieceType::ROOK,
        PieceType::QUEEN,
        PieceType::KING
    }) {
        int typeIndex = static_cast<int>(pt);

        // Evaluate White pieces
        {
            Bitboard bbWhite = board.pieces(pt, Color::WHITE);
            while (bbWhite) {
                int sq = bbWhite.pop();
                score += baseValues[typeIndex];
                score += pieceSquareTable[typeIndex][sq];
            }
        }

        // Evaluate Black pieces
        {
            Bitboard bbBlack = board.pieces(pt, Color::BLACK);
            while (bbBlack) {
                int sq = bbBlack.pop();
                score -= baseValues[typeIndex];
                score -= pieceSquareTable[typeIndex][mirrorSquare(sq)];
            }
        }
    }

    // Adjust score if Black is to move
    if (board.sideToMove() == Color::BLACK) {
        score = -score;
    }

    return score;
}

} // namespace chess
