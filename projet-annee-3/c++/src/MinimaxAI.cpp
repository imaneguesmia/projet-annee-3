#include "chess/MinimaxAI.hpp"
#include "chess.hpp"

#define MATE_SCORE 10e7
namespace chess {

MinimaxAI::MinimaxAI(int depth) : searchDepth(depth) {}

Move MinimaxAI::getMove(Board& board) {
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
    // Vérifier si la partie est terminée
    auto [gameResult, resultDetails] = board.isGameOver();

    // Échec et mat
    if (gameResult == GameResultReason::CHECKMATE) {
        // Si c'est un mat pour le joueur actuel, retournez une valeur négative
        // Sinon, retournez une valeur positive
        return (resultDetails == GameResult::LOSE) ? (-MATE_SCORE + depth) : (MATE_SCORE - depth);
    }

    // Partie nulle (pat, répétition, etc.)
    if (gameResult == GameResultReason::STALEMATE || 
        gameResult == GameResultReason::INSUFFICIENT_MATERIAL || 
        gameResult == GameResultReason::THREEFOLD_REPETITION) {
        return 0; // Égalité
    }

    // Si la profondeur est atteinte ou si la partie n'est pas terminée, évaluez la position
    if (depth == 0) {
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
    // Point en fonction de la position des pièces
    score += pieceSquareHeuristic(board);

    return score;
}
int leastSignificantBitIndex(uint64_t n) {
    // Algorithm by Kim Walisch.
    // https://www.chessprogramming.org/BitScan#KimWalisch

    static const uint64_t debruijn_hash_64 = 0x03f79d71b4cb0a89ULL;

    static const int index_64[64] = {
        0, 47,  1, 56, 48, 27,  2, 60,
        57, 49, 41, 37, 28, 16,  3, 61,
        54, 58, 35, 52, 50, 42, 21, 44,
        38, 32, 29, 23, 17, 11,  4, 62,
        46, 55, 26, 59, 40, 36, 15, 53,
        34, 51, 20, 43, 31, 22, 10, 45,
        25, 39, 14, 33, 19, 30,  9, 24,
        13, 18,  8, 12,  7,  6,  5, 63
    };

    // Black magic hashery
    return index_64[((n ^ (n-1)) * debruijn_hash_64) >> 58];
};

int MinimaxAI::pieceSquareHeuristic(const Board& board){
    int score = 0;

    for (auto pt : {PieceType::PAWN, PieceType::KNIGHT, PieceType::BISHOP, PieceType::ROOK, PieceType::QUEEN,PieceType::KING}) {
        //todo Replace with (PType p_type = PType::FIRST; p_type != PType::OOB; increment_enum(p_type))
        uint64_t pieceBitboard = board.pieces(pt, Color::WHITE).getBits();

        while(pieceBitboard){
            int pos = leastSignificantBitIndex(pieceBitboard);
            pieceBitboard &= pieceBitboard - 1;

            // Flip for black pieces
            // if (!isWhite) pos = 63 - pos;
            
            score += pieceTable[int(pt)][pos];
        }
    }

    return score;
}

}
