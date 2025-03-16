// // #include "chess/NNUEEvaluator.hpp"
// // #include <fstream>
// // #include <stdexcept>
// // #include <cmath>
// // #include <iostream> // debug

// // namespace chess {

// // void NNUEEvaluator::read2DArray(const nlohmann::json& j,
// //                                 const std::string& key,
// //                                 std::vector<float>& arr,
// //                                 int dim0,
// //                                 int dim1)
// // {
// //     const auto& mat = j.at(key);
// //     arr.resize(dim0 * dim1);

// //     for(int i=0; i<dim0; i++) {
// //         const auto& row = mat.at(i);
// //         for(int k=0; k<dim1; k++) {
// //             arr[i*dim1 + k] = row.at(k).get<float>();
// //         }
// //     }
// // }

// // void NNUEEvaluator::read1DArray(const nlohmann::json& j,
// //                                 const std::string& key,
// //                                 std::vector<float>& arr,
// //                                 int dim)
// // {
// //     const auto& vec = j.at(key);
// //     arr.resize(dim);

// //     for(int i=0; i<dim; i++) {
// //         arr[i] = vec.at(i).get<float>();
// //     }
// // }

// // void NNUEEvaluator::loadNetwork(const std::string& jsonPath)
// // {
// //     std::ifstream ifs(jsonPath);
// //     if(!ifs.is_open()) {
// //         throw std::runtime_error("Impossible d'ouvrir le fichier JSON: " + jsonPath);
// //     }
// //     nlohmann::json jnet;
// //     ifs >> jnet;
// //     ifs.close();

// //     // On s'attend à voir :
// //     //  ft.weight : [128, 768]
// //     //  ft.bias   : [128]
// //     //  out.weight: [1, 256]
// //     //  out.bias  : [1]
// //     read2DArray(jnet, "ft.weight",  ftWeight, FT_OUT, INPUT_SIZE);
// //     read1DArray(jnet, "ft.bias",    ftBias,   FT_OUT);
// //     read2DArray(jnet, "out.weight", outWeight, OUTPUT_SIZE, 2*FT_OUT);
// //     read1DArray(jnet, "out.bias",   outBias,   OUTPUT_SIZE);

// //     // Petit log
// //     std::cout << "[NNUEEvaluator::loadNetwork] Réseau chargé depuis " << jsonPath
// //               << ".\n"
// //               << "   ftWeight.size()=" << ftWeight.size()
// //               << ", ftBias.size()=" << ftBias.size()
// //               << ", outWeight.size()=" << outWeight.size()
// //               << ", outBias.size()=" << outBias.size()
// //               << std::endl;

// //     // (Optionnel) Afficher un petit exemple de poids pour diagnostiquer
// //     std::cout << "[DEBUG] Exemples ftWeight[0..4] =";
// //     for(int i=0; i<5; i++) {
// //         std::cout << " " << ftWeight[i];
// //     }
// //     std::cout << "\n[DEBUG] Exemples outWeight[0..4] =";
// //     for(int i=0; i<5; i++) {
// //         std::cout << " " << outWeight[i];
// //     }
// //     std::cout << std::endl;
// // }

// // float NNUEEvaluator::evaluate(const Board& board)
// // {
// //     // (Optionnel) Afficher la FEN si possible (pour vérifier la position)
// //     // std::cout << "\n[NNUEEvaluator::evaluate] FEN=" << board.toFEN() << std::endl;

// //     // 1) Construire les vecteurs d'entrée : board_stm, board_nstm
// //     std::array<float, INPUT_SIZE> board_stm  = {0.f};
// //     std::array<float, INPUT_SIZE> board_nstm = {0.f};

// //     int stmColor = static_cast<int>(board.sideToMove()); // 0=White, 1=Black
// //     std::cout << "[NNUEEvaluator::evaluate] sideToMove=" << stmColor << std::endl;

// //     // Parcourir toutes les cases
// //     for (Square sq = Square::underlying::SQ_A1; sq <= Square::underlying::SQ_H8; ++sq)
// //     {
// //         auto piece = board.at(sq);
// //         if (piece.type() == PieceType::NONE) {
// //             continue; // pas de pièce
// //         }

// //         int colorInt = piece.color();        // 0=White, 1=Black
// //         int ptypeInt = static_cast<int>(piece.type()); // 0..5 (pawn..king)
// //         int sqIndex  = sq.index();           // 0..63

// //         // Calcul de l'index en perspective
// //         int index = featureIndexPerspective(stmColor, colorInt, ptypeInt, sqIndex);

// //         // Debug
// //         std::cout << "  => Piece c=" << colorInt
// //                   << ", t=" << ptypeInt
// //                   << ", sq=" << sqIndex
// //                   << ", perspectiveIndex=" << index
// //                   << ((colorInt==stmColor) ? " (STM)\n" : " (NSTM)\n");

// //         // Sélection du vecteur stm ou nstm
// //         if(colorInt == stmColor) {
// //             if(index<0 || index>=INPUT_SIZE) {
// //                 std::cerr << "[ERROR] index out of range => " << index << std::endl;
// //             }
// //             board_stm[index] = 1.f;
// //         } else {
// //             if(index<0 || index>=INPUT_SIZE) {
// //                 std::cerr << "[ERROR] index out of range => " << index << std::endl;
// //             }
// //             board_nstm[index] = 1.f;
// //         }
// //     }

// //     // 2) Première couche (ft) : dimension FT_OUT=128
// //     //    sumStm = ftBias[i] + ∑(board_stm[j]*ftWeight[i,j])
// //     //    clamp(0,1)
// //     std::vector<float> stm_ft(FT_OUT, 0.f);
// //     std::vector<float> nstm_ft(FT_OUT, 0.f);

// //     for(int i=0; i<FT_OUT; i++) {
// //         float sumStm  = ftBias[i];
// //         float sumNstm = ftBias[i];

// //         for(int j=0; j<INPUT_SIZE; j++) {
// //             sumStm  += board_stm[j]  * ftWeight[i*INPUT_SIZE + j];
// //             sumNstm += board_nstm[j] * ftWeight[i*INPUT_SIZE + j];
// //         }

// //         // clamp(0,1)
// //         if(sumStm < 0.f) sumStm=0.f; else if(sumStm>1.f) sumStm=1.f;
// //         if(sumNstm<0.f) sumNstm=0.f; else if(sumNstm>1.f) sumNstm=1.f;

// //         stm_ft[i]  = sumStm;
// //         nstm_ft[i] = sumNstm;
// //     }

// //     // Debug : affichage partiel des neurones du 1er layer
// //     std::cout << "  [First layer partial] stm_ft[0..4]= ";
// //     for(int k=0; k<5 && k<FT_OUT; k++) {
// //         std::cout << stm_ft[k] << " ";
// //     }
// //     std::cout << ",   nstm_ft[0..4]= ";
// //     for(int k=0; k<5 && k<FT_OUT; k++) {
// //         std::cout << nstm_ft[k] << " ";
// //     }
// //     std::cout << std::endl;

// //     // 3) Deuxième couche (out) => outVal
// //     //    outVal = outBias[0] + ∑(stm_ft[i]*outWeight[i]) + ∑(nstm_ft[i]*outWeight[128 + i])
// //     float outVal = outBias[0];
// //     std::cout << "  [DEBUG] outVal initialisé à outBias[0]=" << outBias[0] << "\n";

// //     // Affichage complet ou partiel ?
// //     // Ici, on propose d'afficher tout, mais si c'est trop verbeux, limiter à i<10
// //     std::cout << "  [DEBUG] Accumulation stm_ft =>\n";
// //     for(int i=0; i<FT_OUT; i++) {
// //         float contrib = stm_ft[i] * outWeight[i];
// //         float oldVal  = outVal;
// //         outVal += contrib;

// //         std::cout << "    i="<<i
// //                   <<", stm_ft[i]="<<stm_ft[i]
// //                   <<", outWeight[i]="<<outWeight[i]
// //                   <<", +contrib="<< contrib
// //                   <<" => outVal: "<<oldVal<<" -> "<< outVal << std::endl;
// //     }

// //     std::cout << "  [DEBUG] Accumulation nstm_ft =>\n";
// //     for(int i=0; i<FT_OUT; i++) {
// //         float contrib = nstm_ft[i] * outWeight[FT_OUT + i];
// //         float oldVal  = outVal;
// //         outVal += contrib;

// //         std::cout << "    i="<<i
// //                   <<", nstm_ft[i]="<<nstm_ft[i]
// //                   <<", outWeight[FT_OUT + i]="<< outWeight[FT_OUT + i]
// //                   <<", +contrib="<< contrib
// //                   <<" => outVal: "<<oldVal<<" -> "<< outVal << std::endl;
// //     }

// //     // 4) Conversion finale => cpValue
// //     static constexpr float SCALE = 400.f;
// //     float cpValue = outVal * SCALE;

// //     if(stmColor == 1) { // si Noir
// //         cpValue = -cpValue;
// //     }

// //     // Debug final
// //     float sigm = 1.f / (1.f + std::exp(-outVal));
// //     std::cout << "  => Raw outVal=" << outVal
// //               << ", Sigmoid=" << sigm
// //               << ", finalCP=" << cpValue
// //               << "  (trait="<< stmColor << ")\n";

// //     return cpValue;
// // }

// // } // namespace chess




// #include "chess/NNUEEvaluator.hpp"
// #include <stdexcept>
// #include <iostream>
// #include <string>
// #include <cstring>
// #include <sys/socket.h> // socket, connect, send, recv
// #include <arpa/inet.h>  // sockaddr_in, inet_pton
// #include <unistd.h>     // close()

// NNUEEvaluator::NNUEEvaluator() {
// }

// NNUEEvaluator::~NNUEEvaluator() {
//     // Si vous aviez besoin de stopper le serveur ou nettoyer, c'est ici
// }

// void NNUEEvaluator::loadNetwork(const std::string& networkPath) {
//     // Pour l'instant, on se contente de stocker ce chemin
//     // si on voulait, on pourrait lancer le serveur Python ici
//     m_networkPath = networkPath;

//     std::cout << "[NNUEEvaluator] Le chemin du réseau est: " << networkPath << std::endl;
//     std::cout << "[NNUEEvaluator] Assurez-vous que le serveur Python est lancé:\n"
//               << "    python nnue_server.py --model-path " << networkPath
//               << " --port " << m_port << std::endl;
// }

// int NNUEEvaluator::evaluate(const chess::Board& board) {
//     // 1. Extraire la FEN depuis la classe Board (vous avez board.toFen() ?)
//     std::string fen = board.getFen();

//     // 2. Connecter au serveur Python
//     int sockfd = ::socket(AF_INET, SOCK_STREAM, 0);
//     if (sockfd < 0) {
//         throw std::runtime_error("NNUEEvaluator: Impossible de créer la socket");
//     }

//     sockaddr_in serv_addr;
//     std::memset(&serv_addr, 0, sizeof(serv_addr));
//     serv_addr.sin_family = AF_INET;
//     serv_addr.sin_port   = htons(m_port);

//     if (inet_pton(AF_INET, m_host.c_str(), &serv_addr.sin_addr) <= 0) {
//         close(sockfd);
//         throw std::runtime_error("NNUEEvaluator: Adresse invalide");
//     }

//     if (connect(sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
//         close(sockfd);
//         throw std::runtime_error(
//             "NNUEEvaluator: Échec de connexion au serveur Python (avez-vous lancé nnue_server.py ?)"
//         );
//     }

//     // 3. Envoyer la FEN (terminée par \n)
//     fen.push_back('\n');
//     if (::send(sockfd, fen.c_str(), fen.size(), 0) < 0) {
//         close(sockfd);
//         throw std::runtime_error("NNUEEvaluator: Erreur d'envoi de la FEN");
//     }

//     // 4. Lire la réponse (score en ASCII, ex: "42\n")
//     char buffer[128];
//     int nbytes = ::recv(sockfd, buffer, sizeof(buffer) - 1, 0);
//     if (nbytes <= 0) {
//         close(sockfd);
//         throw std::runtime_error("NNUEEvaluator: Aucune réponse du serveur");
//     }
//     buffer[nbytes] = '\0';

//     close(sockfd);

//     // 5. Convertir la réponse en entier
//     int score_cp = 0;  // centipawns
//     try {
//         score_cp = std::stoi(buffer);
//     } catch (...) {
//         // En cas d'erreur de parsing
//         score_cp = 0;
//     }
    
//     return score_cp;
// }

#include "chess/NNUEEvaluator.hpp"
#include "chess.hpp" // pour chess::Board (supposé avoir .getFen() etc.)
#include <stdexcept>
#include <iostream>
#include <string>
#include <cstring>
#include <unistd.h>     // close(), read()
#include <arpa/inet.h>  // sockaddr_in, inet_pton

NNUEEvaluator::NNUEEvaluator() : m_sockfd(-1), m_port(5555) {}

NNUEEvaluator::~NNUEEvaluator() {
    // Fermer la socket si ouverte
    if (m_sockfd >= 0) {
        ::close(m_sockfd);
        m_sockfd = -1;
    }
}

void NNUEEvaluator::loadNetwork(const std::string& networkPath,
                                const std::string& host,
                                int port) {
    m_networkPath = networkPath;
    m_host = host;
    m_port = port;

    // 1. Créer la socket
    m_sockfd = ::socket(AF_INET, SOCK_STREAM, 0);
    if (m_sockfd < 0) {
        throw std::runtime_error("[NNUEEvaluator] Impossible de créer la socket");
    }

    // 2. Se connecter au serveur Python
    sockaddr_in serv_addr;
    std::memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port   = htons(m_port);

    if (inet_pton(AF_INET, m_host.c_str(), &serv_addr.sin_addr) <= 0) {
        ::close(m_sockfd);
        m_sockfd = -1;
        throw std::runtime_error("[NNUEEvaluator] Adresse invalide");
    }

    if (connect(m_sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
        ::close(m_sockfd);
        m_sockfd = -1;
        throw std::runtime_error(
            "[NNUEEvaluator] Échec de connexion au serveur Python");
    }

    std::cout << "[NNUEEvaluator] Connecté au serveur "
              << m_host << ":" << m_port << " pour le réseau: " << networkPath << std::endl;
}

int NNUEEvaluator::evaluate(const chess::Board& board) {
    if (m_sockfd < 0) {
        throw std::runtime_error("[NNUEEvaluator] Socket non connectée");
    }

    // 1. Extraire la FEN
    std::string fen = board.getFen();

    // 2. Envoyer la FEN + \n
    fen.push_back('\n');
    ssize_t sent = ::send(m_sockfd, fen.c_str(), fen.size(), 0);
    if (sent < 0) {
        throw std::runtime_error("[NNUEEvaluator] Échec d'envoi de la FEN");
    }

    // 3. Lire la réponse (une ligne, ex: "123\n")
    std::string line = readLine();
    if (line.empty()) {
        // pas de réponse
        throw std::runtime_error("[NNUEEvaluator] Pas de réponse du serveur");
    }

    // Convertir la réponse en entier
    int score_cp = 0;
    try {
        score_cp = std::stoi(line);
    } catch (...) {
        score_cp = 0;
    }
    return score_cp;
}

// Fonction qui lit une ligne terminée par '\n' depuis m_sockfd
std::string NNUEEvaluator::readLine() {
    std::string result;
    char c;
    while (true) {
        ssize_t n = ::recv(m_sockfd, &c, 1, 0);
        if (n <= 0) {
            // Connexion fermée ou erreur
            break;
        }
        if (c == '\n') {
            break;
        }
        result.push_back(c);
    }
    return result;
}
