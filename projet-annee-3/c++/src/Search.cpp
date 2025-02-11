#include "chess/Search.hpp"
#include <algorithm> // std::max, etc.
#include "chess/MoveOrdering.hpp"
#include "chess/QuiescenceSearch.hpp"

namespace chess {

Nicolas::Nicolas(int depth)
    : searchDepth(depth)
{
    // Rien de spécial ici, on fait juste
    // l'initialisation par défaut de nos membres :
    // - transpositionTable
    // - searchHeuristics
    // - evaluator
}

Move Nicolas::getMove(Board& board)
{
    /*
      Cette fonction effectue l'iterative deepening de 1 à searchDepth,
      avec une fenêtre d'aspiration autour du meilleur score trouvé à l'itération précédente.
    */
    Move bestMove  = Move::NO_MOVE;
    int  bestScore = -INF;

    int alphaGlobal = -INF;
    int betaGlobal  = +INF;

    for (int currentDepth = 1; currentDepth <= searchDepth; ++currentDepth)
    {
        // Fenêtre d’aspiration autour de bestScore, sauf pour la première itération
        int alpha = (currentDepth == 1) ? alphaGlobal : bestScore - ASP_WIN;
        int beta  = (currentDepth == 1) ? betaGlobal  : bestScore + ASP_WIN;

        int score = negamax(board, currentDepth, alpha, beta, /*ply=*/0);

        // Si on a un fail-low ou fail-high, on refait la recherche avec la fenêtre complète
        if (score <= alpha || score >= beta) {
            score = negamax(board, currentDepth, -INF, +INF, /*ply=*/0);
        }

        bestScore = score;

        // On va chercher le meilleur coup dans la TT
        auto it = transpositionTable.lookup(board.hash());
        if (it.has_value()) {
            bestMove = it->bestMove;
        }
    }

    return bestMove;
}

int Nicolas::negamax(Board& board, int depth, int alpha, int beta, int ply)
{
    const std::uint64_t zKey = board.hash();

    // Vérification répétition ou 50 coups
    if (ply > 0) {
        if (board.isRepetition(1) || board.isHalfMoveDraw()) {
            return 0; // On renvoie un score neutre
        }
    }

    // Lookup dans la table de transposition
    auto ttEntryOpt = transpositionTable.lookup(zKey);
    if (ttEntryOpt.has_value()) {
        const TTEntry& entry = *ttEntryOpt;
        if (entry.depth >= depth) {
            // On vérifie le bound
            if (entry.bound == Bound::EXACT) {
                return entry.score;
            }
            else if (entry.bound == Bound::LOWER && entry.score > alpha) {
                alpha = entry.score;
            }
            else if (entry.bound == Bound::UPPER && entry.score < beta) {
                beta = entry.score;
            }
            if (alpha >= beta) {
                return entry.score;
            }
        }
    }

    // Vérification si game over (mat, pat, etc.)
    auto [gameResultReason, gameResult] = board.isGameOver();
    if (gameResultReason != GameResultReason::NONE) {
        return evaluateTerminal(gameResultReason, gameResult, ply);
    }

    // Si on atteint depth <= 0 => quiescence
    if (depth <= 0) {
        return quiescenceSearch(board, alpha, beta, ply, evaluator, moveOrdering);
    }

    // Génération de tous les coups
    Movelist moves;
    movegen::legalmoves<movegen::MoveGenType::ALL>(moves, board);

    if (moves.empty()) {
        // Pas de coup légal, c'est probablement un pat ou un mat
        return evaluator.evaluate(board); // ou un fallback
    }

    // On cherche un "pvMove" éventuel depuis la TT
    Move ttBestMove = Move::NO_MOVE;
    if (ttEntryOpt.has_value()) {
        ttBestMove = ttEntryOpt->bestMove;
    }

    // On trie les coups (MVV-LVA, killers, etc.)
    moveOrdering.orderMoves(moves, board, ply, ttBestMove);

    int bestValue = -INF;
    int alphaOrig = alpha;
    Move bestMove = Move::NO_MOVE;

    for (auto& move : moves) {
        board.makeMove(move);
        int val = -negamax(board, depth - 1, -beta, -alpha, ply+1);
        board.unmakeMove(move);

        if (val > bestValue) {
            bestValue = val;
            bestMove  = move;
        }
        if (bestValue > alpha) {
            alpha = bestValue;
        }
        if (alpha >= beta) {
            // Mise à jour killers
            moveOrdering.updateKillers(move, ply);

            // Mise à jour history
            moveOrdering.updateHistory(board, move, depth);

            break;
        }
    }

    // Stockage dans la TT
    Bound bound;
    if (bestValue <= alphaOrig) {
        bound = Bound::UPPER;  // fail-low
    }
    else if (bestValue >= beta) {
        bound = Bound::LOWER;  // fail-high
    }
    else {
        bound = Bound::EXACT;  // exact
    }

    TTEntry newEntry{bestValue, depth, bound, bestMove};
    transpositionTable.store(zKey, newEntry);

    return bestValue;
}

int Nicolas::evaluateTerminal(GameResultReason reason,
                                 GameResult result,
                                 int ply) const
{
    // Si c'est un mat, on renvoie ±(MATE_SCORE - ply)
    if (reason == GameResultReason::CHECKMATE) {
        if (result == GameResult::WIN) {
            // Côté to-move a fait mat
            return +MATE_SCORE - ply;
        } else {
            // Côté to-move s'est fait mater
            return -MATE_SCORE + ply;
        }
    }
    // Stalemate, etc. => 0
    return 0;
}

} // namespace chess
