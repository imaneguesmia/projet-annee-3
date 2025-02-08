#pragma once

#include <iostream>
#include "Player.hpp"

namespace chess {

    class HumanPlayer : public Player {
    public:
        Move getMove(const Board& board) override;
    };

} // namespace chess
