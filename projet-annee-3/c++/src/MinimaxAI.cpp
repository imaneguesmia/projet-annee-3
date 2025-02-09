#include "chess/MinimaxAI.hpp"
#include "chess.hpp"
namespace chess {

MinimaxAI::MinimaxAI(int depth) : searchDepth(depth) {}

Move MinimaxAI::getMove(const Board& board) {
    Movelist moves;
    movegen::legalmoves<movegen::MoveGenType::ALL>(moves, board);

    if (moves.empty()) {
        return Move::NO_MOVE;
    }

    Move bestMove = moves[0];
    int bestScore = -999999;
    int alpha = -999999;
    int beta =  999999;

    for (const Move& move : moves) {
        Board newBoard = board;
        newBoard.makeMove(move);
        int score = -minimax(newBoard, searchDepth - 1, -beta, -alpha);

        if (score > bestScore) {
            bestScore = score;
            bestMove = move;
        }
        alpha = std::max(alpha, score);
        if (alpha >= beta) break;
    }
    return bestMove;
}

int MinimaxAI::minimax(Board& board, int depth, int alpha, int beta) {
    if (depth == 0 || board.isGameOver().first != GameResultReason::NONE) {
        return evaluate(board);
    }

    Movelist moves;
    movegen::legalmoves<movegen::MoveGenType::ALL>(moves, board);

    int bestScore = -999999;
    for (const Move& move : moves) {
        Board newBoard = board;
        newBoard.makeMove(move);
        int score = -minimax(newBoard, depth - 1, -beta, -alpha);
        bestScore = std::max(bestScore, score);
        alpha = std::max(alpha, score);
        if (alpha >= beta) break;
    }
    return bestScore;
}

int MinimaxAI::evaluate(const Board& board) {
    static const std::unordered_map<PieceType, int, PieceType::Hash> pieceValues = {
        {PieceType::PAWN,   100},
        {PieceType::KNIGHT, 300},
        {PieceType::BISHOP, 300},
        {PieceType::ROOK,   500},
        {PieceType::QUEEN,  900},
        {PieceType::KING,   10000}
    };

    int score = 0;

    // Évaluer le matériel
    for (auto pt : {PieceType::PAWN, PieceType::KNIGHT, PieceType::BISHOP, PieceType::ROOK, PieceType::QUEEN}) {
        score += board.pieces(pt, Color::WHITE).count() * pieceValues.at(pt);
        score -= board.pieces(pt, Color::BLACK).count() * pieceValues.at(pt);
    }

    // Malus pour éviter les répétitions
    constexpr int BACKTRACK_PENALTY = 100;

    if (board.isRepetition(2)) {
        if (board.sideToMove() == Color::WHITE) {
            score -= BACKTRACK_PENALTY;
        } else {
            score += BACKTRACK_PENALTY;
        }
    }

    // Bonus pour inciter aux échecs
    constexpr int CHECK_BONUS = 150;

    if (board.inCheck() && board.sideToMove() == Color::BLACK) {
        score += CHECK_BONUS;
    } else if (board.inCheck() && board.sideToMove() == Color::WHITE) {
        score -= CHECK_BONUS;
    }

    return score;
}

int MinimaxAI::pieceSquareHeuristic(const Board& board){
    // Piece-Square Tables (simplified values)
    const int pawnTable[8][8] = {
        {  0,   0,   0,   0,   0,   0,   0,   0 },
        { 50,  50,  50,  50,  50,  50,  50,  50 },
        { 10,  10,  20,  30,  30,  20,  10,  10 },
        {  5,   5,  10,  25,  25,  10,   5,   5 },
        {  0,   0,   0,  20,  20,   0,   0,   0 },
        {  5,  -5, -10,   0,   0, -10,  -5,   5 },
        {  5,  10,  10, -20, -20,  10,  10,   5 },
        {  0,   0,   0,   0,   0,   0,   0,   0 }
    };

    const int knightTable[8][8] = {
        {-50, -40, -30, -30, -30, -30, -40, -50 },
        {-40, -20,   0,   0,   0,   0, -20, -40 },
        {-30,   0,  10,  15,  15,  10,   0, -30 },
        {-30,   5,  15,  20,  20,  15,   5, -30 },
        {-30,   0,  15,  20,  20,  15,   0, -30 },
        {-30,   5,  10,  15,  15,  10,   5, -30 },
        {-40, -20,   0,   5,   5,   0, -20, -40 },
        {-50, -40, -30, -30, -30, -30, -40, -50 }
    };

    const int bishopTable[8][8] = {
        {-20, -10, -10, -10, -10, -10, -10, -20},
        {-10,   0,   0,   0,   0,   0,   0, -10},
        {-10,   0,   5,  10,  10,   5,   0, -10},
        {-10,   5,   5,  10,  10,   5,   5, -10},
        {-10,   0,  10,  10,  10,  10,   0, -10},
        {-10,  10,  10,  10,  10,  10,  10, -10},
        {-10,   5,   0,   0,   0,   0,   5, -10},
        {-20, -10, -10, -10, -10, -10, -10, -20}
    };

    const int rookTable[8][8] = {
        
    }
}

}
