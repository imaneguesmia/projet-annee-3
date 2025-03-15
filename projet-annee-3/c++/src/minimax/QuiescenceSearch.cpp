#include "QuiescenceSearch.hpp"
// #include "chess.hpp"

#include "MoveOrdering.hpp"
#include "Evaluator.hpp"

// class UnmakeMove;

#include "../logic/game.hpp"

/**
 * @brief Performs a quiescence search to evaluate only capture moves, preventing horizon effects.
 */
int quiescenceSearch(ExtendedGameData& game,
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
    std::vector<Move> captures = game.getCurrentLegals(true);
    // movegen::legalmoves<movegen::MoveGenType::CAPTURE>(captures, game.getBoard());

    // Assign score to moves using heuristics (MVV-LVA, etc.)
    moveOrdering.scoreMoves(captures, game.getBoard(), ply, Move::NO_MOVE, game.getCurrentPlayer());

    for (int moveIndex = 0; moveIndex < captures.size(); moveIndex++) {
        moveOrdering.pickNextMove(captures, moveIndex);
        Move capture = captures[moveIndex];

        std::cout << "quiescence 1\n";

        game.move(capture);
        std::cout << "made move = " << capture << "\n";
        const UnmakeMove& unmake_move = game.getMostRecentUnmakeMove();
        std::cout << "unmake move = "<<unmake_move<<"\n";
        int score = -quiescenceSearch(game, -beta, -alpha, ply + 1, evaluator, moveOrdering);
        std::cout << "quiescence 4\n";
        game.unmakeMoveOnBoard(unmake_move);
        std::cout << "quiescence 5\n";

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
