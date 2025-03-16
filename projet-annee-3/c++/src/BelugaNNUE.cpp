#include "chess/BelugaNNUE.hpp"
#include <algorithm>
#include "chess/MoveOrdering.hpp"
#include "chess/QuiescenceSearchNNUE.hpp"

namespace chess {

BelugaNNUE::BelugaNNUE(const std::string& networkPath)
    : searchDepth(4), // Default search depth of 4
      evaluator()
{
    evaluator.loadNetwork(networkPath);  // Load the network after construction
}

Move BelugaNNUE::getMove(Board& board)
{
    Movelist moves;
    movegen::legalmoves(moves, board);

    if (moves.empty()) {
        return Move(Move::NO_MOVE);
    }

    // Initialize search with aspiration windows
    int alpha = -INF;
    int beta = INF;
    int score;
    Move bestMove = Move(Move::NO_MOVE);

    // evaluator.resetNetwork(board);

    // Start with aspiration windows
    int delta = ASP_WIN;
    int guess = 0; // Initial guess
    
    // Iterative deepening with aspiration windows
    for (int depth = 1; depth <= searchDepth; ++depth) {
        if (depth >= 4) { // Use aspiration windows only for deeper searches
            alpha = guess - delta;
            beta = guess + delta;
        }

        while (true) {
            score = negamax(board, depth, alpha, beta, 0);

            // If score is within bounds, accept it
            if (score > alpha && score < beta) {
                guess = score;
                break;
            }

            // If score is outside bounds, widen the window and retry
            delta *= 2;
            if (score <= alpha) {
                alpha = std::max(-INF, score - delta);
            } else {
                beta = std::min(INF, score + delta);
            }
        }

        // Get the best move from the transposition table
        auto ttEntry = transpositionTable.lookup(board.hash());
        if (ttEntry.has_value()) {
            std::cout << "evaluation: " << ttEntry->score << std::endl;
            bestMove = ttEntry->bestMove;
        }
    }

    return bestMove;
}

int BelugaNNUE::negamax(Board& board, int depth, int alpha, int beta, int ply)
{
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
        return quiescenceSearchNNUE(board, alpha, beta, ply, evaluator, moveOrdering);
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

    // Assign a score to moves based on heuristics (MVV-LVA, killer moves, history..)
    moveOrdering.scoreMoves(moves, board, ply, ttBestMove);

    int bestValue = -INF;
    int alphaOrig = alpha;
    Move bestMove = Move::NO_MOVE;

    for (int moveIndex = 0; moveIndex < moves.size(); moveIndex++) {
        moveOrdering.pickNextMove(moves, moveIndex);  // Bring the best move to index 'moveIndex'
        Move move = moves[moveIndex];

        // Update NNUE state before making the move
        // evaluator.updateForMove(board, move);
        board.makeMove(move);
        // evaluator.resetNetwork(board);

        
        int val = -negamax(board, depth - 1, -beta, -alpha, ply + 1);
        
        board.unmakeMove(move);
        // Restore the previous accumulator state instead of resetting
        // evaluator.restoreAccumulator();
        // evaluator.resetNetwork(board);

        if (val > bestValue) {
            bestValue = val;
            bestMove = move;
        }
        if (bestValue > alpha) {
            alpha = bestValue;
        }
        if (alpha >= beta) {
            if (!board.isCapture(move)) {
                moveOrdering.updateKillers(move, ply);
                moveOrdering.updateHistory(board, move, depth);
            }
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

int BelugaNNUE::evaluateTerminal(GameResultReason reason, GameResult result, int ply) const
{
    if (reason == GameResultReason::CHECKMATE) {
        return (result == GameResult::WIN) ? +MATE_SCORE - ply : -MATE_SCORE + ply;
    }
    return 0;
}

} // namespace chess 