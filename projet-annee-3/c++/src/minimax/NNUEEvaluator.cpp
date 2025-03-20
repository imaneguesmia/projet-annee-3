#include "NNUEEvaluator.hpp"
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

int NNUEEvaluator::evaluate(const Board& board, Player player) {
    if (m_sockfd < 0) {
        throw std::runtime_error("[NNUEEvaluator] Socket non connectée");
    }

    // 1. Extraire la FEN (position uniquement sans infos supplémentaires)
    std::string fen = board.getPositionString();

    // 3. Envoyer la FEN + \n
    fen.push_back('\n');
    ssize_t sent = ::send(m_sockfd, fen.c_str(), fen.size(), 0);
    if (sent < 0) {
        throw std::runtime_error("[NNUEEvaluator] Échec d'envoi de la FEN");
    }

    // 4. Lire la réponse
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
    
    // Si le joueur est noir, inverser le score
    if (player == Player::Black) {
        score_cp = -score_cp;
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