#pragma once

#include "chess.hpp"

namespace chess {

    class Player {
    public:
        virtual ~Player() = default;

        /// Retourne le coup que le joueur souhaite jouer
        virtual Move getMove(const Board& board) = 0;
    };

} // namespace chess
