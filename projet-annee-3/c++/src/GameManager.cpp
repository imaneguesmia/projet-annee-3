#include "chess/GameManager.hpp"
#include "chess.hpp"
#include <iostream>
#include <chrono>

namespace chess {

    GameManager::GameManager(Player* whitePlayer, Player* blackPlayer)
        : white(whitePlayer), black(blackPlayer) {}

    void GameManager::playGame() {
        while (board.isGameOver().first == GameResultReason::NONE) {
            board.displayBoard();
            Player* currentPlayer = (board.sideToMove() == Color::WHITE) ? white : black;

            // Measure move selection time
            auto start = std::chrono::high_resolution_clock::now();
            Move move = currentPlayer->getMove(board);
            auto end = std::chrono::high_resolution_clock::now();

            auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
            std::cout << "Move selection time: " << duration << " ms" << std::endl;

            // If no move is returned, the game is over
            if (move == Move::NO_MOVE) {
                std::cout << "Game over." << std::endl;
                break;
            }

            board.makeMove(move);
            std::cout << "Move played: " << move << std::endl;
        }

        // Display final game result
        std::cout << "Game over.\nFinal board state:\n";
        board.displayBoard();
        std::cout << "Game result reason: " << board.isGameOver().first << std::endl;
        std::cout << "Game result: " << board.isGameOver().second << std::endl;
    }

    void GameManager::displayBoardState() {
        std::cout << "FEN: " << board.getFen() << std::endl;
    }

} // namespace chess
