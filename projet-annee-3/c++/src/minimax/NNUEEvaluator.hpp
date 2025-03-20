#pragma once

#include "IEvaluator.hpp"
#include "../logic/bitboard.hpp"
#include "../logic/board.hpp"
#include "../logic/position.hpp"
#include "../logic/player.hpp"
#include "../logic/piece.hpp"

#include <string>
#include <sys/socket.h> // Pour le type de socket
#include <netinet/in.h>

/**
 * @class NNUEEvaluator
 * @brief Evaluator that uses a neural network (NNUE) for position evaluation.
 * Communicates with a Python server that runs the actual neural network.
 */
class NNUEEvaluator : public IEvaluator {
public:
    /**
     * @brief Constructor initializes the evaluator with default settings.
     */
    NNUEEvaluator();
    
    /**
     * @brief Destructor closes any open connections.
     */
    ~NNUEEvaluator() override;

    /**
     * @brief Loads a neural network model and connects to the Python server.
     * @param networkPath Path to the neural network model file.
     * @param host Host address of the Python server.
     * @param port Port number of the Python server.
     */
    void loadNetwork(const std::string& networkPath,
                     const std::string& host = "127.0.0.1",
                     int port = 5555);

    /**
     * @brief Evaluates the position using the neural network.
     * @param board The current chess board state.
     * @param player The player being evaluated (WHITE or BLACK).
     * @return The evaluation score in centipawns from the perspective of the player.
     */
    int evaluate(const Board& board, Player player) override;

private:
    std::string m_networkPath;
    std::string m_host;
    int m_port;
    int m_sockfd; // Socket file descriptor
    
    /**
     * @brief Reads a line terminated by '\n' from the socket.
     * @return The line read from the socket.
     */
    std::string readLine();
}; 