#include "chess/HumanPlayer.hpp"
#include "chess.hpp"
namespace chess {

    Move HumanPlayer::getMove(const Board& board) {
        std::string moveStr;
        std::cout << "Entrez votre coup (UCI) : ";
        std::cin >> moveStr;
        return uci::uciToMove(board,moveStr);
    }

}
