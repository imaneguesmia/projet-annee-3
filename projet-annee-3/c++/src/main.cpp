#include "chess/GameManager.hpp"
#include "chess/MinimaxAI.hpp"
#include "chess/MinimaxAI2.hpp"
#include "chess/MinimaxAI3.hpp"
#include "chess/HumanPlayer.hpp"
#include "chess/Search.hpp"

int main() {
    // ai vs human
    chess::MinimaxAI2 betterai(5);
    chess::MinimaxAI2 betterrai(5);
    chess::HumanPlayer human;
    chess::Beluga theBest(5);

    // ai vs ai
     chess::MinimaxAI badai(3);
     chess::MinimaxAI ai(5);

    chess::GameManager game(&betterai, &theBest);
    game.playGame();

    return 0;
}
