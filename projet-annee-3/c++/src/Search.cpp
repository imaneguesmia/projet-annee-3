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

        int score = negamax(board, currentDepth, alpha, beta, /*ply=*/0, /*isPV=*/true);

        // If we have a fail-low or fail-high, redo search with full window
        if (score <= alpha || score >= beta) {
            score = negamax(board, currentDepth, -INF, +INF, /*ply=*/0, /*isPV=*/true);
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

/**
 * @brief Checks if a move gives check to the opponent's king.
 * @param board The current board state.
 * @param move The move to check.
 * @return True if the move gives check, false otherwise.
 */
bool Beluga::givesCheck(Board& board, const Move move) {
    // Make the move
    board.makeMove(move);
    
    // Check if the opponent's king is in check
    bool isInCheck = board.inCheck();
    
    // Unmake the move
    board.unmakeMove(move);
    
    // Return the result
    return isInCheck;
}

int Beluga::negamax(Board& board, int depth, int alpha, int beta, int ply, bool isPV)
{
    /**
     * @brief Executes the NegaMax algorithm with alpha-beta pruning.
     * Uses transposition tables, move ordering heuristics, and pruning techniques.
     */

    const std::uint64_t zKey = board.hash();
    const bool inCheck = board.inCheck();

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

    // Futility pruning at frontier nodes (depth == 1)
    // Skip if in check or in PV node
    const int staticEval = evaluator.evaluate(board);
    bool skipQuiets = false;
    
    if (depth <= FUTILITY_DEPTH && !inCheck && !isPV) {
        // Calculate futility margin based on depth
        const int futilityMargin = FUTILITY_MARGIN_BASE + depth * FUTILITY_MARGIN_DEPTH;
        
        // If static eval + margin can't raise alpha, consider pruning quiet moves
        if (staticEval + futilityMargin <= alpha) {
            skipQuiets = true;
        }
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
    int movesSearched = 0;

    // Late Move Pruning (LMP) threshold
    const int lmpThreshold = LMP_BASE + depth * depth;

    for (int moveIndex = 0; moveIndex < moves.size(); moveIndex++) {
        moveOrdering.pickNextMove(moves, moveIndex);  // Bring the best move to index 'moveIndex'
        Move move = moves[moveIndex];
        
        const bool isCapture = board.isCapture(move);
        const bool moveGivesCheck = this->givesCheck(board, move);
        
        // Late Move Pruning (LMP)
        // Skip quiet moves after a certain threshold if not in check and not in PV node
        if (!isPV && !inCheck && !isCapture && !moveGivesCheck && 
            depth <= LMP_DEPTH && movesSearched >= lmpThreshold) {
            continue;
        }
        
        // Futility Pruning - Skip quiet moves if they're unlikely to improve alpha
        if (skipQuiets && !isCapture && !moveGivesCheck) {
            continue;
        }

        board.makeMove(move);
        
        int val;
        
        // Late Move Reduction (LMR)
        // Reduce search depth for later quiet moves
        if (movesSearched >= LMR_MOVES_THRESHOLD && 
            depth >= LMR_DEPTH_THRESHOLD && 
            !inCheck && !isCapture && !moveGivesCheck && !isPV) {
            
            // Calculate reduction based on depth and number of moves searched
            int reduction = LMR_BASE;
            
            // Increase reduction for later moves
            reduction += std::min(movesSearched / LMR_MOVES_DIVISOR, LMR_MAX_REDUCTION);
            
            // Ensure we don't reduce too much
            reduction = std::min(depth - 1, reduction);
            
            // Reduced depth search
            val = -negamax(board, depth - 1 - reduction, -alpha - 1, -alpha, ply + 1, false);
            
            // If the reduced search failed high, we need to do a full-depth search
            if (val > alpha) {
                val = -negamax(board, depth - 1, -beta, -alpha, ply + 1, isPV);
            }
        } 
        else {
            // Normal search for important moves or early moves
            val = -negamax(board, depth - 1, -beta, -alpha, ply + 1, isPV && moveIndex == 0);
        }
        
        board.unmakeMove(move);
        movesSearched++;

        if (val > bestValue) {
            bestValue = val;
            bestMove = move;
        }
        if (bestValue > alpha) {
            alpha = bestValue;
        }
        if (alpha >= beta) {
            if (!isCapture) {
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
