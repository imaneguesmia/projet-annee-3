// Could be improved

#pragma once

#include "Evaluator.hpp"
#include "MoveOrdering.hpp"


/**
 * @brief Performs a quiescence search to evaluate stable positions, avoiding horizon effects.
 * @link https://www.chessprogramming.org/Quiescence_Search
 * @param board The current board state.
 * @param alpha Alpha bound for pruning.
 * @param beta Beta bound for pruning.
 * @param ply Current search depth from the root.
 * @param evaluator Static evaluation function.
 * @param moveordering Move ordering heuristics.
 * @return Evaluation score of the position.
 */
int quiescenceSearch(Game& board,
                        int alpha,
                        int beta,
                        int ply,
                        Evaluator& evaluator,
                        MoveOrdering& moveordering);
