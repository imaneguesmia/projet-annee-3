#include "Search.hpp"

#include "MoveOrdering.hpp"
#include "QuiescenceSearch.hpp"

#include "../logic/game.hpp"

#include <algorithm> // std::max

Beluga::Beluga(int depth, std::shared_ptr<const AttackTables> at)
    : searchDepth(depth)
{}

Move Beluga::getMove(ExtendedGameData& board)
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
        auto it = transpositionTable.lookup(board.getHash());
        if (it.has_value()) {
            bestMove = it->bestMove;
        }
        moveOrdering.decayHistory();
    }

    return bestMove;
}

int Beluga::negamax(ExtendedGameData& board, int depth, int alpha, int beta, int ply)
{
    /**
     * @brief Executes the NegaMax algorithm with alpha-beta pruning.
     * Uses transposition tables and move ordering heuristics.
     */

    const std::uint64_t zKey = board.getHash();

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
    GameState current_state = board.getGameState();
    if (current_state != GameState::INGAME) {
        
        return evaluateTerminal(current_state, ply);
    }

    // Quiescence search if depth is zero or negative
    if (depth <= 0) {
        return quiescenceSearch(board, alpha, beta, ply, evaluator, moveOrdering);
    }

    // Generate all legal moves
    std::vector<Move> moves = board.getCurrentLegals();

    if (moves.empty()) {
        return evaluator.evaluate(board.getBoard(), board.getCurrentPlayer());
    }

    // Retrieve a potential best move from transposition table
    Move ttBestMove = Move::NO_MOVE;
    if (ttEntryOpt.has_value()) {
        ttBestMove = ttEntryOpt->bestMove;
    }

    // Assign a score to moves based on heuristics (MVV-LVA, killer moves, history..)
    moveOrdering.scoreMoves(moves, board.getBoard(), ply, ttBestMove, board.getCurrentPlayer());

    int bestValue = -INF;
    int alphaOrig = alpha;
    Move bestMove = Move::NO_MOVE;

    for (int moveIndex = 0; moveIndex < moves.size(); moveIndex++) {
        moveOrdering.pickNextMove(moves, moveIndex);  // Bring the best move to index 'moveIndex'
        Move move = moves[moveIndex];

        board.move(move);
        const UnmakeMove most_recent_unmake_move = board.getMostRecentUnmakeMove();
        int val = -negamax(board, depth - 1, -beta, -alpha, ply + 1);
        board.undo(most_recent_unmake_move);

        if (val > bestValue) {
            bestValue = val;
            bestMove = move;
        }
        if (bestValue > alpha) {
            alpha = bestValue;
        }
        if (alpha >= beta) {
            if (!move.capture) {
                moveOrdering.updateKillers(move, ply);
                moveOrdering.updateHistory(board.getBoard(), move, depth, board.getCurrentPlayer());
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

int Beluga::evaluateTerminal(GameState end_state, int ply) const
{
    /**
     * @brief Evaluates the score for terminal game states.
     * @return MATE_SCORE adjusted for depth, or 0 for stalemate.
     */
    if (end_state == GameState::CHECKMATE) {
        // NOTE: In the old version GameResult can never be WIN here anyway since 
        // we're checking the state for the current player, so no need to check that.

        // return (result == GameResult::WIN) ? +MATE_SCORE - ply : -MATE_SCORE + ply;
        return -MATE_SCORE + ply;
    }
    return 0;
}

int Beluga::getPositionValue(ExtendedGameData& board, Player player) 
{
    int self_eval = evaluator.evaluate(board.getBoard(), player);

    std::cout << "Self eval = " << self_eval << '\n';

    return self_eval;
}