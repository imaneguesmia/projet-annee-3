#include "chess/HumanPlayer.hpp"
#include "chess.hpp"

namespace chess {

    Move HumanPlayer::getMove(Board& board) {
        std::string moveStr;
        std::cout << "Enter your move (UCI format): ";
        std::cin >> moveStr;
        return uci::uciToMove(board, moveStr);
    }

} // namespace chess
