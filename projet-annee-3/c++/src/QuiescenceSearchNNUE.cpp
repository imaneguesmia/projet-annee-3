#include "chess/QuiescenceSearchNNUE.hpp"
#include "chess.hpp"
#include "chess/MoveOrdering.hpp"

namespace chess {

    int quiescenceSearchNNUE(Board& board,
                            int alpha,
                            int beta,
                            int ply,
                            NNUEEvaluator& evaluator,
                            MoveOrdering& moveOrdering)
    {
        // Stand-pat evaluation using NNUE
        int standPat = evaluator.evaluate(board);

        if (standPat >= beta) {
            return beta;  // Beta cutoff
        }
        if (standPat > alpha) {
            alpha = standPat;  // Update alpha if static eval is better
        }

        // Generate only capture moves
        Movelist captures;
        movegen::legalmoves<movegen::MoveGenType::CAPTURE>(captures, board);

        // Score moves for move ordering
        moveOrdering.scoreMoves(captures, board, ply, Move::NO_MOVE);

        for (int moveIndex = 0; moveIndex < captures.size(); moveIndex++) {
            moveOrdering.pickNextMove(captures, moveIndex);
            Move capture = captures[moveIndex];

            // Update NNUE state and make move
            // evaluator.updateForMove(board, capture);
            board.makeMove(capture);

            int score = -quiescenceSearchNNUE(board, -beta, -alpha, ply + 1, evaluator, moveOrdering);

            // Restore position and NNUE state
            board.unmakeMove(capture);
            // evaluator.restoreAccumulator();

            if (score > alpha) {
                alpha = score;
                if (alpha >= beta) {
                    break;  // Beta cutoff
                }
            }
        }

        return alpha;
    }

} // namespace chess