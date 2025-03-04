#ifndef MINIMAXAI2_HPP
#define MINIMAXAI2_HPP

#include "logic/Player.hpp"

#include <unordered_map>
#include <vector>
#include <cstdint> // Pour std::uint64_t

namespace chess {

    class MinimaxAI2 : public Player {
    public:
        explicit MinimaxAI2(int depth);
        Move getMove(Board& board) override;

    private:
        enum Bound { LOWER, UPPER, EXACT };

        struct TTEntry {
            int score;
            int depth;
            Bound bound;
            Move bestMove;
        };

        int searchDepth;
        std::unordered_map<std::uint64_t, TTEntry> transpositionTable; // Correction ici
        std::vector<std::array<Move, 2>> killerMoves;
        std::vector<std::vector<int>> historyHeuristic;

        int negamax(Board& board, int depth, int alpha, int beta, int ply);
        int quiescence(Board& board, int alpha, int beta, int ply);
        void orderMoves(Movelist& moves, Board& board, int ply, Move pvMove);
        void updateKillers(Move move, int ply);
        int evaluateTerminal(GameResultReason result, GameResult details, int ply) const;
        int evaluate(const Board& board);
    };

} // namespace chess

#endif