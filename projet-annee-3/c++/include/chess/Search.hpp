#ifndef CHESS_SEARCH_HPP
#define CHESS_SEARCH_HPP

#include <cstdint>
#include "Player.hpp"
#include "TranspositionTable.hpp"
#include "Evaluator.hpp"
#include "MoveOrdering.hpp"

namespace chess {

    class Nicolas : public Player
    {
    public:
        // Constructeur : on passe la profondeur max souhaitée
        explicit Nicolas(int depth);

        // Méthode principale pour obtenir le meilleur coup
        Move getMove(Board& board) override;

    private:
        // NegaMax (avec alpha–beta) et aspiration windows
        int negamax(Board& board, int depth, int alpha, int beta, int ply);

        // Évalue un etat terminal (mat/stalemate)
        int evaluateTerminal(GameResultReason reason, GameResult result, int ply) const;

    private:
        int               searchDepth;         // Profondeur maximale
        TranspositionTable transpositionTable; // Table de transposition
        MoveOrdering moveOrdering;   // Gestion des heuristiques (killers, history)
        Evaluator          evaluator;          // Évaluation statique

        static constexpr int INF       = 100000000; // Limite "infinie"
        static constexpr int MATE_SCORE= 1000000;   // Score de base pour un mat
        static constexpr int ASP_WIN   = 50;        // Fenêtre d’aspiration (±50)
    };

} // namespace chess

#endif // CHESS_SEARCH_HPP
