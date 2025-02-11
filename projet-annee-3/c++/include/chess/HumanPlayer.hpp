#pragma once

#include <iostream>
#include "Player.hpp"

namespace chess {

    /**
     * @class HumanPlayer
     * @brief Represents a human-controlled chess player.
     */
    class HumanPlayer : public Player {
    public:
        /**
         * @brief Gets the move from user input in UCI format.
         * @param board The current chess board state.
         * @return The move chosen by the player.
         */
        Move getMove(Board& board) override;
    };

} // namespace chess
