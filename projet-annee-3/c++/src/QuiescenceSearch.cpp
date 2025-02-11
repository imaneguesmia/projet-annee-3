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
        // stand pat = évaluation statique (sans faire de coup)
        int standPat = evaluator.evaluate(board);

        if (standPat >= beta) {
            return beta;
        }
        if (standPat > alpha) {
            alpha = standPat;
        }

        // On ne génère que les captures
        Movelist captures;
        movegen::legalmoves<movegen::MoveGenType::CAPTURE>(captures, board);

        // On peut réutiliser la même fonction de tri,
        // en mettant pvMove = NO_MOVE, etc.
        moveordering.orderMoves(captures, board, ply, Move::NO_MOVE);

        for (const auto& capture : captures) {
            board.makeMove(capture);
            int score = -quiescenceSearch(board, -beta, -alpha, ply+1, evaluator, moveordering);
            board.unmakeMove(capture);

            if (score > standPat) {
                standPat = score;
                if (score > alpha) {
                    alpha = score;
                }
                if (alpha >= beta) {
                    break;
                }
            }
        }

        return alpha;
    }

} // namespace chess
