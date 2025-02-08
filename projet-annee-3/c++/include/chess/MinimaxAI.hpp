#pragma once

#include "Player.hpp"

namespace chess {

    class MinimaxAI : public Player {
    public:
        explicit MinimaxAI(int depth = 3);
        Move getMove(const Board& board) override;

    private:
        int searchDepth;

        int minimax(Board& board, int depth, int alpha, int beta);
        int evaluate(const Board& board);
    };

} // namespace chess
