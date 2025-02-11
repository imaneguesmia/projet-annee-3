#ifndef CHESS_QUIESCENCESEARCH_HPP
#define CHESS_QUIESCENCESEARCH_HPP

#include "chess.hpp"
#include "Evaluator.hpp"
#include "MoveOrdering.hpp"

namespace chess {

    /*
      Fonctions pour la quiescence search
    */
    int quiescenceSearch(Board& board,
                         int alpha,
                         int beta,
                         int ply,
                         Evaluator& evaluator,
                         MoveOrdering& moveordering);

} // namespace chess

#endif // CHESS_QUIESCENCESEARCH_HPP
