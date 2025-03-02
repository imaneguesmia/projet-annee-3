#include "chess/QuiescenceSearch.hpp"
// #include "chess.hpp"

#include "chess/MoveOrdering.hpp"
#include "chess/Evaluator.hpp"

#include "../logic/game.hpp"


/**
 * @brief Performs a quiescence search to evaluate only capture moves, preventing horizon effects.
 */
int quiescenceSearch(Game& game,
                        int alpha,
                        int beta,
                        int ply,
                        Evaluator& evaluator,
                        MoveOrdering& moveOrdering)
{
    // Stand-pat evaluation (static evaluation without moving)
    int standPat = evaluator.evaluate(game.getBoard(), game.getCurrentPlayer());

    if (standPat >= beta) {
        return beta; // Beta cutoff (fail-hard)
    }
    if (standPat > alpha) {
        alpha = standPat; // Update alpha if the static eval is better
    }

    // Generate only capture moves
    std::vector<Move> captures = game.getCurrentLegals();
    // movegen::legalmoves<movegen::MoveGenType::CAPTURE>(captures, game.getBoard());

    // Assign score to moves using heuristics (MVV-LVA, etc.)
    moveOrdering.scoreMoves(captures, game.getBoard(), ply, Move::NO_MOVE, game.getCurrentPlayer());

    for (int moveIndex = 0; moveIndex < captures.size(); moveIndex++) {
        moveOrdering.pickNextMove(captures, moveIndex);
        Move capture = captures[moveIndex];

        game.move(capture);
        int score = -quiescenceSearch(game, -beta, -alpha, ply + 1, evaluator, moveOrdering);
        game.undoLastMove();

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
