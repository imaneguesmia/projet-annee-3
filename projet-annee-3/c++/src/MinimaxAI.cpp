#include "chess/MinimaxAI.hpp"
#include "chess.hpp"

#define MATE_SCORE 10e7
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

int MinimaxAI::pieceSquareHeuristic(const Board& board){
    int score = 0;

    for (auto pt : {PieceType::PAWN, PieceType::KNIGHT, PieceType::BISHOP, PieceType::ROOK, PieceType::QUEEN}) {
        //todo Replace with (PType p_type = PType::FIRST; p_type != PType::OOB; increment_enum(p_type))
        uint64_t pieceBitboard = board.pieces(pt, Color::WHITE).getBits();

        while(pieceBitboard){
            int pos = __builtin_ctzll(pieceBitboard); //todo or leastSignificantIndex
            pieceBitboard &= pieceBitboard - 1;

            // Flip for black pieces
            // if (!isWhite) pos = 63 - pos;
            
            score += pieceTable[int(pt)][pos];
        }
    }
    // King handling
    int pt = int(PieceType::KING);
    int64_t pieceBitboard = board.pieces(PieceType::KING, Color::WHITE).getBits();
    int pos = __builtin_ctzll(pieceBitboard);

    auto [gameResult, _] = board.isGameOver();

    // Game is not over
    if (gameResult == GameResultReason::NONE) {
        score += pieceTable[pt][pos];
    // Game is over
    }else {
        score += pieceTable[pt][++pos]; 
    }
    return score;
}

}
