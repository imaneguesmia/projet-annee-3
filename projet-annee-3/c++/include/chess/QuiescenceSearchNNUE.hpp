#pragma once

#include "chess.hpp"
#include "NNUEEvaluator.hpp"
#include "MoveOrdering.hpp"

namespace chess {

    /**
     * @brief Performs a quiescence search with NNUE evaluation to evaluate stable positions.
     * Similar to regular quiescence search but maintains NNUE state during search.
     * 
     * @param board The current board state
     * @param alpha Alpha bound for pruning
     * @param beta Beta bound for pruning
     * @param ply Current search depth from root
     * @param evaluator NNUE evaluator instance
     * @param moveOrdering Move ordering heuristics
     * @return Evaluation score of the position
     */
    int quiescenceSearchNNUE(Board& board,
                            int alpha,
                            int beta,
                            int ply,
                            NNUEEvaluator& evaluator,
                            MoveOrdering& moveOrdering);

} // namespace chess