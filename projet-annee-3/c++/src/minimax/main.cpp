#include "chess/GameManager.hpp"
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
    chess::Beluga theBest2(5);

    chess::GameManager game(&theBest, &betterai);  // White: 238.148 ms per move, Black: 435.13 ms per move
    game.playGame();

    return 0;
}
