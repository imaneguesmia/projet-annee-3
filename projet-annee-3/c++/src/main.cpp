#include "chess/GameManager.hpp"
#include "chess/MinimaxAI.hpp"
#include "chess/MinimaxAI2.hpp"
#include "chess/MinimaxAI3.hpp"
#include "chess/HumanPlayer.hpp"
#include "chess/Search.hpp"
#include "chess/BelugaNNUE.hpp"

int main() {
    // ai vs human
    chess::MinimaxAI2 betterai(5);
    chess::MinimaxAI2 betterrai(5);
    chess::HumanPlayer human;
    chess::Beluga theBest(4);
    chess::Beluga theBest2(5);

    // Create BelugaNNUE instance with neural network
    chess::BelugaNNUE belugaNNUE("../../marlinflow/trainer/nn/beluga_v1.json");

    // ai vs ai
    chess::MinimaxAI badai(3);
    chess::MinimaxAI ai(5);

    // You can uncomment one of these lines to play different matches:
    
    // Human vs BelugaNNUE (you play as White)
    chess::GameManager game(&theBest,&belugaNNUE);
    
    // Or BelugaNNUE vs Human (you play as Black)
    // chess::GameManager game(&belugaNNUE, &human);
    
    // Or test BelugaNNUE against classic Beluga
    // chess::GameManager game(&belugaNNUE, &theBest);
    
    // Or let two BelugaNNUE instances play against each other
    // chess::BelugaNNUE belugaNNUE2(5, "marlinflow/trainer/nn/first_training.json");
    // chess::GameManager game(&belugaNNUE, &belugaNNUE2);

    // chess::GameManager game(&betterai, &betterrai);  //White: 238.148 ms per move, Black: 435.13 ms per move
    game.playGame();

    return 0;
}
