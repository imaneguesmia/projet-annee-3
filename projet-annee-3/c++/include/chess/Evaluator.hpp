#ifndef CHESS_EVALUATOR_HPP
#define CHESS_EVALUATOR_HPP

#include "chess.hpp"

namespace chess {

    /*
      Classe responsable de l'évaluation statique
      (matériel, tables de placement, etc.)
    */
    class Evaluator
    {
    public:
        Evaluator() = default;

        // Renvoie le score du point de vue du camp "White" par convention
        // (ou 0 si position égale).
        // Si sideToMove == BLACK, on inversera le score (selon votre choix).
        int evaluate(const Board& board);

    private:
        int mirrorSquare(int sq) const;
    };

} // namespace chess

#endif // CHESS_EVALUATOR_HPP
