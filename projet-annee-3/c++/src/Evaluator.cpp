#include "chess/Evaluator.hpp"
#include "chess.hpp"
#include <array>

namespace chess {

// On peut définir ces tables en "static" ou "constexpr" en global
static const int baseValues[] = {
    /* 0 : PAWN   */  100,
    /* 1 : KNIGHT */  300,
    /* 2 : BISHOP */  300,
    /* 3 : ROOK   */  500,
    /* 4 : QUEEN  */  900,
    /* 5 : KING   */  10000,
    /* 6 : NONE   */  0
};

// Indices : 0=Pawn, 1=Knight, 2=Bishop, 3=Rook, 4=Queen, 5=King, 6=KingEndgame(?)
static const int pieceSquareTable[7][64] =
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
    // King (midgame)
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
    // King (endgame) – vous pourriez distinguer midgame/endgame
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

int Evaluator::mirrorSquare(int sq) const
{
    // On suppose que 0..63 indexe les cases (a1=0, h1=7, a2=8, etc.)
    // Pour retourner la "vue" miroir (blanc vs noir), on fait sq ^ 56.
    // a1 (0) -> a8 (56), etc.
    return sq ^ 56;
}

int Evaluator::evaluate(const Board& board)
{
    int score = 0;

    // On liste les types qui nous intéressent
    for (auto pt : {
        PieceType::PAWN,
        PieceType::KNIGHT,
        PieceType::BISHOP,
        PieceType::ROOK,
        PieceType::QUEEN,
        PieceType::KING
    })
    {
        int typeIndex = static_cast<int>(pt);

        // Pièces blanches
        {
            Bitboard bbWhite = board.pieces(pt, Color::WHITE);
            while (bbWhite) {
                int sq = bbWhite.pop();
                score += baseValues[typeIndex];
                score += pieceSquareTable[typeIndex][sq];
            }
        }
        // Pièces noires
        {
            Bitboard bbBlack = board.pieces(pt, Color::BLACK);
            while (bbBlack) {
                int sq = bbBlack.pop();
                score -= baseValues[typeIndex];
                // On “mirroir” la case pour le black
                int mirrored = mirrorSquare(sq);
                score -= pieceSquareTable[typeIndex][mirrored];
            }
        }
    }

    // Option : vous pouvez inverser le score si sideToMove == BLACK
    // pour rester cohérent à un point de vue "to move".
    if (board.sideToMove() == Color::BLACK) {
        score = -score;
    }

    return score;
}

} // namespace chess
