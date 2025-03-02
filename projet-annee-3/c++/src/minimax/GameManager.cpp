#include "chess/GameManager.hpp"
#include "chess.hpp"
#include <iostream>
#include <chrono>

namespace chess {

    GameManager::GameManager(Player* whitePlayer, Player* blackPlayer)
        : white(whitePlayer), black(blackPlayer),
          whiteTotalTime(0), blackTotalTime(0),
          whiteMoveCount(0), blackMoveCount(0) {}

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

            // Accumulate time and count moves per player
            if (board.sideToMove() == Color::WHITE) {
                whiteTotalTime += duration;
                whiteMoveCount++;
            } else {
                blackTotalTime += duration;
                blackMoveCount++;
            }

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

        // Compute and display average move times
        double avgWhiteTime = (whiteMoveCount > 0) ? static_cast<double>(whiteTotalTime) / whiteMoveCount : 0.0;
        double avgBlackTime = (blackMoveCount > 0) ? static_cast<double>(blackTotalTime) / blackMoveCount : 0.0;

        std::cout << "\n--- Average Move Time ---\n";
        std::cout << "White: " << avgWhiteTime << " ms per move (" << whiteMoveCount << " moves)\n";
        std::cout << "Black: " << avgBlackTime << " ms per move (" << blackMoveCount << " moves)\n";
    }

    void GameManager::displayBoardState() {
        std::cout << "FEN: " << board.getFen() << std::endl;
    }

} // namespace chess
