#include "Search.hpp"

#include "MoveOrdering.hpp"
#include "QuiescenceSearch.hpp"

#include "../logic/game.hpp"

#include <algorithm> // std::max
#include <chrono> 

Beluga::Beluga(EvaluatorSettings settings)
    : searchDepth(settings.depth)
{
    // Create the appropriate evaluator based on the type
    if (settings.type == EvaluatorType::NNUE) {
        auto nnueEval = std::make_unique<NNUEEvaluator>();

        std::cout << "Network path = " << settings.networkPath << '\n';
        
        // If a network path is provided, load it
        if (!settings.networkPath.empty()) {
            nnueEval->loadNetwork(settings.networkPath);
        }
        
        evaluator = std::move(nnueEval);
    } else {
        // Default to standard evaluator
        evaluator = std::make_unique<Evaluator>(settings);
    }
}

Move Beluga::getMove(ExtendedGameData& board)
{
    /**
     * @brief Implements iterative deepening search up to the set depth.
     * Uses aspiration windows for more efficient search.
     */

    auto start = std::chrono::high_resolution_clock::now();
    Move bestMove = Move::NO_MOVE;
    int bestScore = -INF;

    int alphaGlobal = -INF;
    int betaGlobal  = +INF;

    for (int currentDepth = 1; currentDepth <= searchDepth; ++currentDepth)
    {
        // Aspiration window around bestScore (except for first iteration)
        int alpha = (currentDepth == 1) ? alphaGlobal : bestScore - ASP_WIN;
        int beta  = (currentDepth == 1) ? betaGlobal  : bestScore + ASP_WIN;

        int score = negamax(board, currentDepth, alpha, beta, /*ply=*/0, /*isPV=*/true, /*nullMoveAllowed=*/true);

        // If we have a fail-low or fail-high, redo search with full window
        if (score <= alpha || score >= beta) {
            score = negamax(board, currentDepth, -INF, +INF, /*ply=*/0, /*isPV=*/true, /*nullMoveAllowed=*/true);
        }

        bestScore = score;

        // Retrieve best move from transposition table
        auto it = transpositionTable.lookup(board.getHash());
        if (it.has_value()) {
            bestMove = it->bestMove;
        }
        moveOrdering.decayHistory();
    }
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    std::cout << "Search time: " << duration.count() << " milliseconds" << std::endl;
    return bestMove;
}

/**
 * @brief Checks if a move gives check to the opponent's king.
 * @param board The current board state.
 * @param move The move to check.
 * @return True if the move gives check, false otherwise.
 */
bool Beluga::givesCheck(ExtendedGameData& board, const Move& move) {
    // Make the move
    board.move(move);
    const UnmakeMove unmake_move = board.getMostRecentUnmakeMove();
    
    // Check if the opponent's king is in check
    bool is_in_check = board.isCurrentlyInCheck();
    
    // Unmake the move
    board.undo(unmake_move);
    
    // Return the result
    return is_in_check;
}

int Beluga::negamax(ExtendedGameData& board, int depth, int alpha, int beta, int ply, bool isPV, bool nullMoveAllowed)
{
    /**
     * @brief Executes the NegaMax algorithm with alpha-beta pruning.
     * Uses transposition tables, move ordering heuristics, and pruning techniques.
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
        return quiescenceSearch(board, alpha, beta, ply, *evaluator, moveOrdering);
    }

    // Get the current player
    const Player current_player = board.getCurrentPlayer();
    
    // Check if in check
    const bool in_check = board.isCurrentlyInCheck();

    // Static evaluation for pruning decisions
    const Board& chess_board = board.getBoard();
    const int static_eval = evaluator->evaluate(chess_board, current_player);
    
    // Reverse Futility Pruning (Static Null Move Pruning)
    // Skip if in check, in PV node, or at high depth
    if (!in_check && !isPV && depth <= RFP_DEPTH) {
        const int rfp_margin = RFP_MARGIN * depth;
        if (static_eval - rfp_margin >= beta) {
            return static_eval; // Position is so good that even with a margin, we're above beta
        }
    }

    // Null Move Pruning
    // Skip if in check, in PV node, or at low depth, or zugzwang-prone positions
    if (nullMoveAllowed && !in_check && !isPV && depth >= NULL_MOVE_MIN_DEPTH && static_eval >= beta) {
        // Since we can't directly make a null move, we'll create an artificial position
        // by making and then undoing two different moves, effectively skipping a turn

        // Generate legal moves
        const std::vector<Move>& moves = board.getCurrentLegals();
        if (!moves.empty()) {
            // Make and undo the first move
            Move firstMove = moves[0];
            board.move(firstMove);
            const UnmakeMove firstUnmakeMove = board.getMostRecentUnmakeMove();
            
            // Get legal moves for the opponent
            const std::vector<Move>& opponentMoves = board.getCurrentLegals();
            if (!opponentMoves.empty()) {
                // Make and undo one of the opponent's moves
                Move opponentMove = opponentMoves[0];
                board.move(opponentMove);
                const UnmakeMove opponentUnmakeMove = board.getMostRecentUnmakeMove();
                
                // Now search with reduced depth
                int null_score = -negamax(board, depth - NULL_MOVE_REDUCTION - 1, -beta, -beta + 1, ply + 1, false, false);
                
                // Unmake the opponent's move
                board.undo(opponentUnmakeMove);
                
                // Unmake the first move
                board.undo(firstUnmakeMove);
                
                // If the null move search fails high, we can prune this node
                if (null_score >= beta) {
                    return beta; // Null move cutoff
                }
            } else {
                // If the opponent has no legal moves, unmake the first move
                board.undo(firstUnmakeMove);
            }
        }
    }

    // Futility pruning flag - will be set if static eval is far below alpha
    bool skip_quiets = false;
    
    if (depth <= FUTILITY_DEPTH && !in_check && !isPV) {
        // Calculate futility margin based on depth
        const int futility_margin = FUTILITY_MARGIN_BASE + depth * FUTILITY_MARGIN_DEPTH;
        
        // If static eval + margin can't raise alpha, consider pruning quiet moves
        if (static_eval + futility_margin <= alpha) {
            skip_quiets = true;
        }
    }

    // Generate all legal moves
    std::vector<Move> moves = board.getCurrentLegals();

    if (moves.empty()) {
        return evaluator->evaluate(chess_board, current_player);
    }

    // Retrieve a potential best move from transposition table
    Move ttBestMove = Move::NO_MOVE;
    if (ttEntryOpt.has_value()) {
        ttBestMove = ttEntryOpt->bestMove;
    }

    // Assign a score to moves based on heuristics (MVV-LVA, killer moves, history..)
    moveOrdering.scoreMoves(moves, chess_board, ply, ttBestMove, current_player);

    int bestValue = -INF;
    int alphaOrig = alpha;
    Move bestMove = Move::NO_MOVE;
    int movesSearched = 0;

    // Late Move Pruning (LMP) threshold
    const int lmp_threshold = LMP_BASE + depth * depth;

    for (int moveIndex = 0; moveIndex < moves.size(); moveIndex++) {
        moveOrdering.pickNextMove(moves, moveIndex);  // Bring the best move to index 'moveIndex'
        Move move = moves[moveIndex];
        
        const bool is_capture = move.capture;
        const bool gives_check = this->givesCheck(board, move);
        
        // Late Move Pruning (LMP)
        // Skip quiet moves after a certain threshold if not in check and not in PV node
        if (!isPV && !in_check && !is_capture && !gives_check && 
            depth <= LMP_DEPTH && movesSearched >= lmp_threshold) {
            continue;
        }
        
        // Futility Pruning - Skip quiet moves if they're unlikely to improve alpha
        if (skip_quiets && !is_capture && !gives_check) {
            continue;
        }

        board.move(move);
        const UnmakeMove most_recent_unmake_move = board.getMostRecentUnmakeMove();
        
        int val;
        
        // Late Move Reduction (LMR)
        // Reduce search depth for later quiet moves
        if (movesSearched >= LMR_MOVES_THRESHOLD && 
            depth >= LMR_DEPTH_THRESHOLD && 
            !in_check && !is_capture && !gives_check && !isPV) {
            
            // Calculate reduction based on depth and number of moves searched
            int reduction = LMR_BASE;
            
            // Increase reduction for later moves
            reduction += std::min(movesSearched / LMR_MOVES_DIVISOR, LMR_MAX_REDUCTION);
            
            // Ensure we don't reduce too much
            reduction = std::min(depth - 1, reduction);
            
            // Reduced depth search
            val = -negamax(board, depth - 1 - reduction, -alpha - 1, -alpha, ply + 1, false, true);
            
            // If the reduced search failed high, we need to do a full-depth search
            if (val > alpha) {
                val = -negamax(board, depth - 1, -beta, -alpha, ply + 1, isPV, true);
            }
        } 
        else {
            // Normal search for important moves or early moves
            val = -negamax(board, depth - 1, -beta, -alpha, ply + 1, isPV && moveIndex == 0, true);
        }
        
        board.undo(most_recent_unmake_move);
        movesSearched++;

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
                moveOrdering.updateHistory(chess_board, move, depth, current_player);
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
    int self_eval = evaluator->evaluate(board.getBoard(), player);

    return self_eval;
}