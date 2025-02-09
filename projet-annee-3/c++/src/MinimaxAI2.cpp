#include "chess/MinimaxAI2.hpp"
#include "chess.hpp"
#include <algorithm>
#include <limits>
#include <unordered_map>
#include <vector>

namespace chess {

constexpr int INF = 10000000;
constexpr int MATE_SCORE = 1000000;

int scoreMove(Move move, const Board & board) {
    if (board.isCapture(move)) {
        // Récupération des pièces concernées
        Piece movingPiece  = board.at(move.from());
        Piece capturedPiece = board.at(move.to());

        // Valeurs associées à chaque type de pièce
        static const std::unordered_map<PieceType, int, PieceType::Hash> pieceValues = {
            {PieceType::PAWN,   100},
            {PieceType::KNIGHT, 300},
            {PieceType::BISHOP, 300},
            {PieceType::ROOK,   500},
            {PieceType::QUEEN,  900},
            {PieceType::KING,   10000}
        };

        // Si la case destination est vide (cas de l'en passant), on considère la victime comme un pion.
        int victimValue = (capturedPiece != Piece())
                          ? pieceValues.at(capturedPiece.type())
                          : pieceValues.at(PieceType::PAWN);
        int attackerValue = pieceValues.at(movingPiece.type());
        return victimValue - attackerValue;
    }
    return 0;
}

MinimaxAI2::MinimaxAI2(int depth) : searchDepth(depth) {}

int MinimaxAI2::quiescence(Board & board, int alpha, int beta) {
    int standPat = evaluate(board);
    if (standPat >= beta)
        return beta;
    if (alpha < standPat)
        alpha = standPat;

    Movelist moves;

    movegen::legalmoves<movegen::MoveGenType::CAPTURE>(moves, board);

    for (Move move : moves) {
        board.makeMove(move);
        int score = -quiescence(board, -beta, -alpha);
        board.unmakeMove(move);
        if (score >= beta)
            return beta;
        if (score > alpha)
            alpha = score;
    }
    return alpha;
}

int MinimaxAI2::negamax(Board & board, int depth, int alpha, int beta) {
    auto [gameResult, details] = board.isGameOver();
    if (depth == 0 || gameResult != GameResultReason::NONE) {
        if (gameResult != GameResultReason::NONE) {
            if (gameResult == GameResultReason::CHECKMATE)
                return -MATE_SCORE + depth;  // Favorise les mats rapides
            else
                return 0;  // Partie nulle (pat ou autre)
        }
        return quiescence(board, alpha, beta);
    }

    Movelist moves;
    movegen::legalmoves<movegen::MoveGenType::ALL>(moves, board);
    if (moves.empty())
        return quiescence(board, alpha, beta);

    std::sort(moves.begin(), moves.end(),
              [&board](Move a, Move b) {
                  return scoreMove(a, board) > scoreMove(b, board);
              });

    int bestScore = -INF;
    for (Move move : moves) {
        board.makeMove(move);
        int score = -negamax(board, depth - 1, -beta, -alpha);
        board.unmakeMove(move);
        if (score > bestScore)
            bestScore = score;
        if (bestScore > alpha)
            alpha = bestScore;
        if (alpha >= beta)
            break;
    }
    return bestScore;
}

Move MinimaxAI2::getMove(Board & board) {
    Movelist moves;
    movegen::legalmoves<movegen::MoveGenType::ALL>(moves, board);
    if (moves.empty())
        return Move::NO_MOVE;

    // On initialise bestMove avec le premier coup disponible.
    Move bestMove = moves[0];
    int bestScore = -INF;

    // Bonus pour privilégier le meilleru coup calculé précédemment
    constexpr int BONUS_PV = 1000000;

    // Itérative deepening de 1 à searchDepth
    for (int currentDepth = 1; currentDepth <= searchDepth; ++currentDepth) {
        movegen::legalmoves<movegen::MoveGenType::ALL>(moves, board);

        // Tri des coups
        std::sort(moves.begin(), moves.end(),
                  [&board, bestMove](Move a, Move b) {
                      int scoreA = scoreMove(a, board) + ((a == bestMove) ? BONUS_PV : 0);
                      int scoreB = scoreMove(b, board) + ((b == bestMove) ? BONUS_PV : 0);
                      return scoreA > scoreB;
                  });

        int alpha = -INF;
        int beta  = INF;
        bestScore = -INF;

        for (Move move : moves) {
            board.makeMove(move);
            int score = -negamax(board, currentDepth - 1, -beta, -alpha);
            board.unmakeMove(move);
            if (score > bestScore) {
                bestScore = score;
                bestMove = move;
            }
            if (score > alpha)
                alpha = score;
            if (alpha >= beta)
                break;
        }
    }
    return bestMove;
}


int MinimaxAI2::evaluate(const Board & board) {
    static const std::unordered_map<PieceType, int, PieceType::Hash> pieceValues = {
        {PieceType::PAWN,   100},
        {PieceType::KNIGHT, 300},
        {PieceType::BISHOP, 300},
        {PieceType::ROOK,   500},
        {PieceType::QUEEN,  900},
        {PieceType::KING,   10000}
    };

    int score = 0;
    for (auto pt : {PieceType::PAWN, PieceType::KNIGHT, PieceType::BISHOP, PieceType::ROOK, PieceType::QUEEN}) {
        score += board.pieces(pt, Color::WHITE).count() * pieceValues.at(pt);
        score -= board.pieces(pt, Color::BLACK).count() * pieceValues.at(pt);
    }

    constexpr int BACKTRACK_PENALTY = 100;
    if (board.isRepetition(2)) {
        score += (board.sideToMove() == Color::WHITE) ? -BACKTRACK_PENALTY : BACKTRACK_PENALTY;
    }

    constexpr int CHECK_BONUS = 150;
    if (board.inCheck()) {
        score += (board.sideToMove() == Color::BLACK) ? CHECK_BONUS : -CHECK_BONUS;
    }

    return score;
}

} // namespace chess
