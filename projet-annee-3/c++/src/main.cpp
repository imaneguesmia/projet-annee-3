#include "chess/GameManager.hpp"
#include "chess/MinimaxAI.hpp"
#include "chess/HumanPlayer.hpp"


int main() {
    // ai vs human
    chess::MinimaxAI ai(5);
    chess::HumanPlayer human;
    // ai vs ai
    // chess::MinimaxAI ai(3);
    // chess::MinimaxAI betterai(5);

    chess::GameManager game(&human, &ai);
    game.playGame();

    return 0;
}
