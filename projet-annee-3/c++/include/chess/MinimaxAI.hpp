#pragma once

#include "Player.hpp"

namespace chess {

    class MinimaxAI : public Player {
    public:
        explicit MinimaxAI(int depth = 3);
        Move getMove(Board& board) override;

    private:
        int searchDepth;

        int negamax(Board& board, int depth, int alpha, int beta);
        int evaluate(const Board& board);
    };

} // namespace chess
