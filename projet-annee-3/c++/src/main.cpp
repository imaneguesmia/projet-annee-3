#include "chess/GameManager.hpp"
#include "chess/MinimaxAI.hpp"
#include "chess/MinimaxAI2.hpp"
#include "chess/HumanPlayer.hpp"

int main() {
    // ai vs human
    chess::MinimaxAI2 betterai(5);
    chess::HumanPlayer human;
    // ai vs ai
     chess::MinimaxAI badai(3);
     chess::MinimaxAI ai(5);

    chess::GameManager game(&human, &betterai);
    game.playGame();

    return 0;
}
