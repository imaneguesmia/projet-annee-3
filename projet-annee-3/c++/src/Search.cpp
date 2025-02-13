#include "chess/Search.hpp"
#include <algorithm> // std::max
#include "chess/MoveOrdering.hpp"
#include "chess/QuiescenceSearch.hpp"

namespace chess {

Beluga::Beluga(int depth)
    : searchDepth(depth)
{
}

Move Beluga::getMove(Board& board)
{
    /**
     * @brief Implements iterative deepening search up to the set depth.
     * Uses aspiration windows for more efficient search.
     */

    Move bestMove = Move::NO_MOVE;
    int bestScore = -INF;

    int alphaGlobal = -INF;
    int betaGlobal  = +INF;

    for (int currentDepth = 1; currentDepth <= searchDepth; ++currentDepth)
    {
        // Aspiration window around bestScore (except for first iteration)
        int alpha = (currentDepth == 1) ? alphaGlobal : bestScore - ASP_WIN;
        int beta  = (currentDepth == 1) ? betaGlobal  : bestScore + ASP_WIN;

        int score = negamax(board, currentDepth, alpha, beta, /*ply=*/0);

        // If we have a fail-low or fail-high, redo search with full window
        if (score <= alpha || score >= beta) {
            score = negamax(board, currentDepth, -INF, +INF, /*ply=*/0);
        }

        bestScore = score;

        // Retrieve best move from transposition table
        auto it = transpositionTable.lookup(board.hash());
        if (it.has_value()) {
            bestMove = it->bestMove;
        }
        moveOrdering.decayHistory();
    }

    return bestMove;
}

int Beluga::negamax(Board& board, int depth, int alpha, int beta, int ply)
{
    /**
     * @brief Executes the NegaMax algorithm with alpha-beta pruning.
     * Uses transposition tables and move ordering heuristics.
     */

    const std::uint64_t zKey = board.hash();

    // Check for repetition or 50-move rule
    if (ply > 0) {
        if (board.isRepetition(1) || board.isHalfMoveDraw()) {
            return 0; // Neutral score
        }
    }

    // Transposition table lookup
    auto ttEntryOpt = transpositionTable.lookup(zKey);
    if (ttEntryOpt.has_value()) {
        const TTEntry& entry = *ttEntryOpt;
        if (entry.depth >= depth) {
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

    // Check if game is over (checkmate, stalemate, etc.)
    auto [gameResultReason, gameResult] = board.isGameOver();
    if (gameResultReason != GameResultReason::NONE) {
        return evaluateTerminal(gameResultReason, gameResult, ply);
    }

    // Quiescence search if depth is zero or negative
    if (depth <= 0) {
        return quiescenceSearch(board, alpha, beta, ply, evaluator, moveOrdering);
    }

    // Generate all legal moves
    Movelist moves;
    movegen::legalmoves<movegen::MoveGenType::ALL>(moves, board);

    if (moves.empty()) {
        return evaluator.evaluate(board);
    }

    // Retrieve a potential best move from transposition table
    Move ttBestMove = Move::NO_MOVE;
    if (ttEntryOpt.has_value()) {
        ttBestMove = ttEntryOpt->bestMove;
    }

    // Order moves based on heuristics (MVV-LVA, killer moves, history..)
    moveOrdering.orderMoves(moves, board, ply, ttBestMove);

    int bestValue = -INF;
    int alphaOrig = alpha;
    Move bestMove = Move::NO_MOVE;

    for (auto& move : moves) {
        board.makeMove(move);
        int val = -negamax(board, depth - 1, -beta, -alpha, ply + 1);
        board.unmakeMove(move);

        if (val > bestValue) {
            bestValue = val;
            bestMove = move;
        }
        if (bestValue > alpha) {
            alpha = bestValue;
        }
        if (alpha >= beta) {
            moveOrdering.updateKillers(move, ply);
            moveOrdering.updateHistory(board, move, depth);
            break;
        }
    }

    // Store result in transposition table
    Bound bound;
    if (bestValue <= alphaOrig) {
        bound = Bound::UPPER;  // fail-low
    } else if (bestValue >= beta) {
        bound = Bound::LOWER;  // fail-high
    } else {
        bound = Bound::EXACT;  // exact value
    }

    TTEntry newEntry{bestValue, depth, bound, bestMove};
    transpositionTable.store(zKey, newEntry);

    return bestValue;
}

int Beluga::evaluateTerminal(GameResultReason reason, GameResult result, int ply) const
{
    /**
     * @brief Evaluates the score for terminal game states.
     * @return MATE_SCORE adjusted for depth, or 0 for stalemate.
     */
    if (reason == GameResultReason::CHECKMATE) {
        return (result == GameResult::WIN) ? +MATE_SCORE - ply : -MATE_SCORE + ply;
    }
    return 0;
}

} // namespace chess
