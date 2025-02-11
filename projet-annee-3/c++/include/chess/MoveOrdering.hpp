#ifndef CHESS_MOVEORDERING_HPP
#define CHESS_MOVEORDERING_HPP

#include <array>
#include <vector>
#include "chess.hpp"

/*
  Cette classe gère :
    - Les killer moves
    - L'history heuristic
    - Le tri des coups (captures, PV move, etc.)
*/
namespace chess {

    class MoveOrdering
    {
    public:
        MoveOrdering();

        // Mise à jour des killer moves
        void updateKillers(Move move, int ply);

        // Mise à jour de l'history
        void updateHistory(const Board& board, Move move, int depth);

        // Accès killers
        Move killerAt(int ply, int index) const;

        // Accès history
        int  historyScore(const Board& board, Move move) const;

        // Fonction de tri des coups
        // (inclut MVV-LVA, killer moves, history, etc.)
        void orderMoves(Movelist& moves,
                        const Board& board,
                        int ply,
                        Move pvMove);

    private:
        // Stockage des killers : max 64 ply, 2 killers
        std::vector<std::array<Move,2>> killerMoves;

        // History heuristic : 16 types × 64 cases
        std::vector<std::vector<int>> historyHeuristic;

        static constexpr int MAX_PLY = 64;
    };

} // namespace chess

#endif // CHESS_MOVEORDERING_HPP
