#include "QuiescenceSearch.hpp"
// #include "chess.hpp"

#include "MoveOrdering.hpp"
#include "Evaluator.hpp"

// class UnmakeMove;

#include "../logic/game.hpp"
#include "../logic/piece.hpp"

/**
 * @brief Performs a quiescence search to evaluate only capture moves, preventing horizon effects.
 */
int quiescenceSearch(ExtendedGameData& game,
                        int alpha,
                        int beta,
                        int ply,
                        IEvaluator& evaluator,
                        MoveOrdering& moveOrdering)
{
    // Stand-pat evaluation (static evaluation without moving)
    const Board& chess_board = game.getBoard();
    const Player current_player = game.getCurrentPlayer();
    int standPat = evaluator.evaluate(chess_board, current_player);

    // Return immediately if the static evaluation is already better than beta
    if (standPat >= beta) {
        return beta; // Beta cutoff (fail-hard)
    }
    
    // Update alpha if the static eval is better
    if (standPat > alpha) {
        alpha = standPat;
    }

    // Generate only capture moves
    std::vector<Move> captures = game.getCurrentLegals(true);

    // Assign score to moves using heuristics (MVV-LVA, etc.)
    moveOrdering.scoreMoves(captures, chess_board, ply, Move::NO_MOVE, current_player);

    for (int moveIndex = 0; moveIndex < captures.size(); moveIndex++) {
        moveOrdering.pickNextMove(captures, moveIndex);
        Move capture = captures[moveIndex];

        game.move(capture);
        const UnmakeMove unmake_move = game.getMostRecentUnmakeMove();
        int score = -quiescenceSearch(game, -beta, -alpha, ply + 1, evaluator, moveOrdering);
        game.undo(unmake_move);

        if (score > standPat) {
            standPat = score;
            if (score > alpha) {
                alpha = score;
            }
            if (alpha >= beta) {
                break; // Beta cutoff (fail-hard pruning)
            }
        }
    }

    return alpha;
}
