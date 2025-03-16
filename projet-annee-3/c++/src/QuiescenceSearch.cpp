#include "chess/QuiescenceSearch.hpp"
#include "chess.hpp"
#include "chess/MoveOrdering.hpp"

namespace chess {

    int quiescenceSearch(Board& board,
                         int alpha,
                         int beta,
                         int ply,
                         Evaluator& evaluator,
                         MoveOrdering& moveOrdering)
    {
        /**
         * @brief Performs a quiescence search to evaluate only capture moves, preventing horizon effects.
         */

        // Stand-pat evaluation (static evaluation without moving)
        int standPat = evaluator.evaluate(board);

        if (standPat >= beta) {
            return beta; // Beta cutoff (fail-hard)
        }
        if (standPat > alpha) {
            alpha = standPat; // Update alpha if the static eval is better
        }

        // Generate only capture moves
        Movelist captures;
        movegen::legalmoves<movegen::MoveGenType::CAPTURE>(captures, board);

        // If no captures are available, return the stand-pat score
        if (captures.empty()) {
            return standPat;
        }

        // Assign score to moves using heuristics (MVV-LVA, etc.)
        moveOrdering.scoreMoves(captures, board, ply, Move::NO_MOVE);

        for (int moveIndex = 0; moveIndex < captures.size(); moveIndex++) {
            moveOrdering.pickNextMove(captures, moveIndex);
            Move capture = captures[moveIndex];

            board.makeMove(capture);
            int score = -quiescenceSearch(board, -beta, -alpha, ply + 1, evaluator, moveOrdering);
            board.unmakeMove(capture);

            if (score > alpha) {
                alpha = score;
                if (alpha >= beta) {
                    break; // Beta cutoff (fail-hard pruning)
                }
            }
        }

        return alpha;
    }

} // namespace chess