#pragma once

#include "chess.hpp"
#include "Player.hpp"
#include <vector>
#include <limits>
#include <unordered_map>
#include <algorithm>

namespace chess {

    class MinimaxAI2 : public Player {
    public:
        explicit MinimaxAI2(int depth);
        Move getMove(Board& board) override;

    private:
        int negamax(Board& board, int depth, int alpha, int beta);
        int quiescence(Board& board, int alpha, int beta);
        int evaluate(const Board& board);
        int searchDepth;
    };

} // namespace chess
