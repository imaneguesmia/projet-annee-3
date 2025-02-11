#pragma once

#include "chess.hpp"

namespace chess {

    /**
     * @class Player
     * @brief Abstract base class representing a chess player.
     */
    class Player {
    public:

        virtual ~Player() = default;

        /**
         * @brief Determines the move the player wants to play.
         * @param board The current chess board state.
         * @return The move chosen by the player.
         */
        virtual Move getMove(Board& board) = 0;
    };

} // namespace chess
