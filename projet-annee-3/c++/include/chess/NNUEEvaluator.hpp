// #pragma once

// #include <string>
// #include <vector>
// #include <array>
// #include "nlohmann/json.hpp"
// #include "chess.hpp"

// // On suppose que vous avez déjà:
// //   - enum class Color : uint8_t { WHITE=0, BLACK=1, ... };
// //   - class Board { ... };
// //   - class Square { ... };
// //   - class PieceType { ... };
// //   - class Piece { ... };

// //   =>  board.sideToMove() -> Color (0=WHITE, 1=BLACK)
// //   =>  board.at(square) -> Piece
// //   =>  piece.type() -> PieceType::underlying (0..5 = Pion..Roi, 6=NONE)
// //   =>  piece.color() -> 0=WHITE, 1=BLACK
// //   =>  square.index() -> [0..63]

// namespace chess {

// // Classe qui stocke et applique un réseau NnBoard768(128)
// // c'est-à-dire dimension d'entrée = 768, hidden=128, sortie=1 (avec cat(stm,nstm)=256).
// class NNUEEvaluator {
// public:
//     static constexpr int INPUT_SIZE  = 768;
//     static constexpr int FT_OUT      = 128;
//     static constexpr int OUTPUT_SIZE = 1;

//     NNUEEvaluator() = default;
//     ~NNUEEvaluator() = default;

//     void loadNetwork(const std::string& jsonPath);
//     float evaluate(const Board& board);

// private:
//     std::vector<float> ftWeight; // [FT_OUT, INPUT_SIZE]
//     std::vector<float> ftBias;   // [FT_OUT]
//     std::vector<float> outWeight;// [OUTPUT_SIZE, 2*FT_OUT] => [1,256]
//     std::vector<float> outBias;  // [OUTPUT_SIZE] => [1]

// private:
//     void read2DArray(const nlohmann::json& j, const std::string& key,
//                      std::vector<float>& arr, int dim0, int dim1);
//     void read1DArray(const nlohmann::json& j, const std::string& key,
//                      std::vector<float>& arr, int dim);
    
//     // Important : fonction qui reproduit la perspective "Rust"
//     // sideToMove=0 => White, 1 => Black
//     // -> flip rank + invert color si c'est noir
//     inline int featureIndexPerspective(int sideToMove, int color, int pieceType, int sqIndex) const {
//         if(sideToMove == 1) {   // Noir
//             sqIndex = 63 - sqIndex;    // flip rank
//             color   = 1 - color;       // invert color
//         }
//         return (color*6 + pieceType)*64 + sqIndex;
//     }
// };

// } // namespace chess


// #pragma once

// #include <string>
// #include "chess.hpp"

// class NNUEEvaluator {
// public:
//     NNUEEvaluator();
//     ~NNUEEvaluator();

//     // Charge un "réseau" (ici, on retient juste le chemin).
//     // Optionnellement, on peut préciser host/port si besoin.
//     void loadNetwork(const std::string& networkPath);

//     // Évalue la position (en centipions) via le serveur Python
//     int evaluate(const chess::Board& board);

// private:
//     std::string m_networkPath;  // chemin vers le .pt (peut servir si on voulait lancer le serveur)
//     std::string m_host = "127.0.0.1";
//     int m_port = 5555;
// };



#pragma once
#include "chess.hpp"
#include <string>
#include <sys/socket.h> // Pour le type de socket
#include <netinet/in.h>

namespace chess {
    class Board;
}

class NNUEEvaluator {
public:
    NNUEEvaluator();
    ~NNUEEvaluator();

    void loadNetwork(const std::string& networkPath,
                     const std::string& host = "127.0.0.1",
                     int port = 5555);

    // Évalue la position en centipions
    int evaluate(const chess::Board& board);

private:
    std::string m_networkPath;
    std::string m_host;
    int m_port;
    int m_sockfd; // on stocke le socket ouvert

    // Petite fonction pour lire une ligne terminée par '\n' depuis m_sockfd
    std::string readLine();
};
