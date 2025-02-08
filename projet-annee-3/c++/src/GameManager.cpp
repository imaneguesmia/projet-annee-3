#include "chess/GameManager.hpp"
#include "chess.hpp"
#include <iostream>

namespace chess {

    GameManager::GameManager(Player* whitePlayer, Player* blackPlayer)
        : white(whitePlayer), black(blackPlayer){}

    void GameManager::playGame() {
        while (board.isGameOver().first == GameResultReason::NONE) {
            board.displayBoard();
            // std::cout << board;
            Player* currentPlayer = (board.sideToMove() == Color::WHITE) ? white : black;
            Move move = currentPlayer->getMove(board);

            if (move == Move::NO_MOVE) {
                std::cout << "Partie terminée." << std::endl;
                break;
            }
            board.makeMove(move);
            std::cout << "Coup joué : " << move << std::endl;
        }

        std::cout << "Partie terminée.\nÉtat final du plateau :\n";
        board.displayBoard();
        std::cout << "Game result reason: " << board.isGameOver().first << std::endl;
        std::cout << "Game result: " << board.isGameOver().second << std::endl;

        // std::cout << board;
    }


    void GameManager::displayBoardState() {
        std::cout << "FEN : " << board.getFen() << std::endl;
    }

}