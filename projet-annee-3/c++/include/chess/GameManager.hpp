#pragma once

#include "Player.hpp"

namespace chess {

    /**
     * @class GameManager
     * @brief Manages and controls the flow of a chess game between two players.
     */
    class GameManager {
    public:
        /**
         * @brief Constructs a GameManager with two players.
         * @param whitePlayer Pointer to the white player.
         * @param blackPlayer Pointer to the black player.
         */
        GameManager(Player* whitePlayer, Player* blackPlayer);

        /**
         * @brief Starts and manages the game loop.
         */
        void playGame();

    private:
        Player* white;
        Player* black;
        Board board;

        /**
         * @brief Displays the current board state in FEN format.
         */
        void displayBoardState();
    };

} // namespace chess
