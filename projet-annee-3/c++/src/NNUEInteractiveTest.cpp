#include "chess/NNUEEvaluator.hpp"
#include "chess.hpp"
#include <iostream>
#include <string>
#include <chrono>
#include <sstream>

using namespace chess;

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <chemin_vers_reseau_nnue>\n";
        return 1;
    }
    
    std::string networkPath = argv[1];
    
    // Initialiser l'évaluateur NNUE
    NNUEEvaluator evaluator;
    try {
        evaluator.loadNetwork(networkPath); // connecte au serveur Python, ou stocke le chemin
    } catch (const std::exception& e) {
        std::cerr << "Erreur lors du chargement du réseau NNUE: " << e.what() << "\n";
        return 1;
    }
    
    // Initialiser l'échiquier
    Board board;
    board.setFen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"); // Position initiale
    
    std::string input;
    
    std::cout << "=== Test Interactif NNUE ===\n";
    std::cout << "Vous pouvez taper :\n"
              << "  - un coup au format UCI (ex: e2e4)\n"
              << "  - un numéro de coup (ex: 1, 2, 3...)\n"
              << "  - la commande \"fen <FEN>\" pour définir la position\n"
              << "  - \"quit\" pour quitter\n\n";
    
    while (true) {
        // Afficher l'échiquier
        board.displayBoard();
        
        // Évaluation NNUE
        int eval = evaluator.evaluate(board);
        float evalPawns = eval / 100.0f;
        std::cout << "Évaluation NNUE: ";
        if (eval == 0) {
            std::cout << "0.00 (égalité)";
        } else if (eval > 0) {
            std::cout << "+" << evalPawns << " (avantage aux blancs)";
        } else {
            std::cout << evalPawns << " (avantage aux noirs)";
        }
        std::cout << "\n\n";
        
        // Vérifier fin de partie
        auto [reason, result] = board.isGameOver();
        if (reason != GameResultReason::NONE) {
            std::cout << "Partie terminée! Raison: " << static_cast<int>(reason) 
                      << ", Résultat: " << static_cast<int>(result) << std::endl;
            break;
        }
        
        // Générer les coups légaux
        Movelist moves;
        movegen::legalmoves(moves, board);
        
        std::cout << "Coups légaux:\n";
        for (int i = 0; i < moves.size(); ++i) {
            std::cout << i + 1 << ". " << moves[i] << " ";
            if ((i + 1) % 5 == 0) std::cout << "\n";
        }
        std::cout << "\n\n";
        
        // Lecture de la commande
        std::cout << "Entrez votre coup (UCI/numéro) ou \"fen <FEN>\" ou \"quit\": ";
        std::getline(std::cin, input);
        
        // Vérifier si l'utilisateur veut quitter
        if (input == "quit" || input == "exit" || input == "q") {
            break;
        }

        // Vérifier si la commande commence par "fen "
        if (input.rfind("fen ", 0) == 0) { 
            // Extraire la FEN
            std::string newFen = input.substr(4);
            try {
                board.setFen(newFen);
                std::cout << "Nouvelle position FEN définie.\n";
            } catch (...) {
                std::cout << "FEN invalide !\n";
            }
            continue; // Revenir en haut de la boucle pour réafficher
        }
        
        // Essayer de lire l'entrée comme un indice de coup
        Move move = Move::NO_MOVE;
        try {
            int moveIndex = std::stoi(input) - 1;
            if (moveIndex >= 0 && moveIndex < moves.size()) {
                move = moves[moveIndex];
            }
        } catch (...) {
            // Sinon essayer comme un coup UCI
            move = uci::uciToMove(board, input);
        }
        
        if (move != Move::NO_MOVE) {
            // Jouer le coup
            auto start = std::chrono::high_resolution_clock::now();
            board.makeMove(move);
            auto end = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
            
            std::cout << "Coup joué: " << move << "\n";
            std::cout << "Temps pour appliquer le coup: " << duration << " µs\n\n";
        } else {
            std::cout << "Commande invalide !\n\n";
        }
    }
    
    std::cout << "Merci d'avoir utilisé le test interactif NNUE!\n";
    return 0;
}
