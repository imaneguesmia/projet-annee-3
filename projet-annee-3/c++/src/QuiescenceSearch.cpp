#include "chess/QuiescenceSearch.hpp"
#include "chess.hpp"
#include "chess/MoveOrdering.hpp"

namespace chess {

    int quiescenceSearch(Board& board,
                         int alpha,
                         int beta,
                         int ply,
                         Evaluator& evaluator,
                         MoveOrdering& moveordering)
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

        // Order moves using heuristics (MVV-LVA, etc.)
        moveordering.orderMoves(captures, board, ply, Move::NO_MOVE);

        for (const auto& capture : captures) {
            board.makeMove(capture);
            int score = -quiescenceSearch(board, -beta, -alpha, ply + 1, evaluator, moveordering);
            board.unmakeMove(capture);

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

} // namespace chess