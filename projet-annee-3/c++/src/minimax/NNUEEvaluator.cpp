#include "NNUEEvaluator.hpp"
#include <stdexcept>
#include <iostream>
#include <string>
#include <cstring>
#include <unistd.h>     // close(), read()
#include <arpa/inet.h>  // sockaddr_in, inet_pton
#include <sys/socket.h> // For socket options
#include <mutex>

NNUEEvaluator::NNUEEvaluator() : m_sockfd(-1), m_port(5555) {}

NNUEEvaluator::~NNUEEvaluator() {
    // Fermer la socket si ouverte
    std::lock_guard<std::mutex> lock(m_socketMutex);
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

    // Établir la connexion initiale
    connectToServer();
}

bool NNUEEvaluator::connectToServer() {
    std::lock_guard<std::mutex> lock(m_socketMutex);
    
    // Fermer la socket si elle existe déjà
    if (m_sockfd >= 0) {
        ::close(m_sockfd);
        m_sockfd = -1;
    }

    // 1. Créer la socket
    m_sockfd = ::socket(AF_INET, SOCK_STREAM, 0);
    if (m_sockfd < 0) {
        std::cerr << "[NNUEEvaluator] Impossible de créer la socket" << std::endl;
        return false;
    }

    // Configurer la socket avec un timeout pour les opérations
    struct timeval timeout;
    timeout.tv_sec = 2;  // 2 secondes de timeout
    timeout.tv_usec = 0;
    if (setsockopt(m_sockfd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout)) < 0) {
        ::close(m_sockfd);
        m_sockfd = -1;
        std::cerr << "[NNUEEvaluator] Impossible de configurer le timeout de réception" << std::endl;
        return false;
    }
    if (setsockopt(m_sockfd, SOL_SOCKET, SO_SNDTIMEO, &timeout, sizeof(timeout)) < 0) {
        ::close(m_sockfd);
        m_sockfd = -1;
        std::cerr << "[NNUEEvaluator] Impossible de configurer le timeout d'envoi" << std::endl;
        return false;
    }

    // 2. Se connecter au serveur Python
    sockaddr_in serv_addr;
    std::memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port   = htons(m_port);

    if (inet_pton(AF_INET, m_host.c_str(), &serv_addr.sin_addr) <= 0) {
        ::close(m_sockfd);
        m_sockfd = -1;
        std::cerr << "[NNUEEvaluator] Adresse invalide" << std::endl;
        return false;
    }

    if (connect(m_sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
        ::close(m_sockfd);
        m_sockfd = -1;
        std::cerr << "[NNUEEvaluator] Échec de connexion au serveur Python" << std::endl;
        return false;
    }

    std::cout << "[NNUEEvaluator] Connecté au serveur "
              << m_host << ":" << m_port << " pour le réseau: " << m_networkPath << std::endl;
    return true;
}

int NNUEEvaluator::evaluate(const Board& board, Player player) {
    // Utiliser un mutex pour protéger l'accès à la socket
    std::lock_guard<std::mutex> lock(m_socketMutex);
    
    // Vérifier si la connexion est active
    if (m_sockfd < 0) {
        // Tenter de se reconnecter
        if (!connectToServer()) {
            // Utiliser une évaluation par défaut en cas d'échec de connexion
            std::cerr << "[NNUEEvaluator] Impossible de se connecter au serveur, utilisation d'une évaluation par défaut" << std::endl;
            return 0;
        }
    }

    // 1. Extraire la FEN (position uniquement sans infos supplémentaires)
    std::string fen = board.getPositionString();

    // 2. Ajouter un retour à la ligne pour terminer la commande
    fen.push_back('\n');
    
    // 3. Envoyer la FEN avec gestion des erreurs et tentatives de reconnexion
    bool sendSuccess = false;
    for (int attempts = 0; attempts < 2 && !sendSuccess; attempts++) {
        ssize_t sent = ::send(m_sockfd, fen.c_str(), fen.size(), 0);
        if (sent < 0) {
            if (attempts == 0) {
                // Première tentative échouée, essayer de se reconnecter
                std::cerr << "[NNUEEvaluator] Échec d'envoi, tentative de reconnexion..." << std::endl;
                if (connectToServer()) {
                    continue;  // Réessayer après reconnexion
                }
            }
            // Échec final
            std::cerr << "[NNUEEvaluator] Échec d'envoi de la FEN après reconnexion" << std::endl;
            return 0;  // Évaluation par défaut
        }
        sendSuccess = true;
    }

    if (!sendSuccess) {
        return 0;  // Évaluation par défaut en cas d'échec
    }

    // 4. Lire la réponse avec gestion des erreurs
    std::string line;
    try {
        line = readLine();
    } catch (const std::exception& e) {
        std::cerr << "[NNUEEvaluator] Erreur lors de la lecture: " << e.what() << std::endl;
        // Fermer la socket en cas d'erreur pour forcer une reconnexion la prochaine fois
        ::close(m_sockfd);
        m_sockfd = -1;
        return 0;  // Évaluation par défaut
    }

    if (line.empty()) {
        // Pas de réponse, fermer la socket pour forcer une reconnexion la prochaine fois
        std::cerr << "[NNUEEvaluator] Pas de réponse du serveur" << std::endl;
        ::close(m_sockfd);
        m_sockfd = -1;
        return 0;  // Évaluation par défaut
    }

    // Convertir la réponse en entier
    int score_cp = 0;
    try {
        score_cp = std::stoi(line);
    } catch (...) {
        std::cerr << "[NNUEEvaluator] Réponse invalide: " << line << std::endl;
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
            // Timeout, connexion fermée ou erreur
            if (n == 0) {
                throw std::runtime_error("Connexion fermée par le serveur");
            } else {
                throw std::runtime_error("Erreur de réception");
            }
        }
        if (c == '\n') {
            break;
        }
        result.push_back(c);
    }
    return result;
} 