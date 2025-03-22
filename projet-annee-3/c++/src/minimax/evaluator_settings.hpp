#pragma once

#include <string>

// Enum for selecting the evaluator type
enum class EvaluatorType {
    Standard,  // Standard heuristic evaluation
    NNUE       // Neural network evaluation
};

// Settings for the evaluator
struct EvaluatorSettings {
    EvaluatorType type = EvaluatorType::Standard; // Default to standard evaluation

    int depth = 5;                                // Maximum search depth.

    // Neural network
    std::string networkPath = "";                 // Path to NNUE network (if using NNUE)
    std::string nnueHost = "127.0.0.1";           // Host for NNUE server
    int nnuePort = 5555;                          // Port for NNUE server

    // Heuristics
    bool material = true;
    bool pieceSquareTable = false;
    bool mobility = true;
    bool pawnStructure = false;
    bool kingSafety = false;
};