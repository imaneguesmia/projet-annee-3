#pragma once

#include "Player.hpp"

namespace chess {

    class GameManager {
    public:
        GameManager(Player* whitePlayer, Player* blackPlayer);
        void playGame();

    private:
        Player* white;
        Player* black;
        Board board;

        void displayBoardState();
    };

} // namespace chess
