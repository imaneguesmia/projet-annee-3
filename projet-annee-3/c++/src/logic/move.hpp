#pragma once

#include "player.hpp"
#include "position.hpp"
#include "piece.hpp"

#include <cstdint>
#include <iostream>

/**
 * @brief Class containing move parameters.
 */
class Move {

public:
    static constexpr int NO_MOVE {0};

    Move() {};
    constexpr Move(const int flag) : p_type(PType::NoneType) {};

    ~Move() {};

    uint8_t source {0};             // The source position of the move (where the piece comes from).
    uint8_t target {0};             // The target position of the move (where the piece is going).
    Player player {Player::OOB};    // The `Player` of the piece that moved. 
    PType p_type {PType::OOB};      // The `PType` of the piece that moved. 
    PType promotion {PType::OOB};   // The `PType` of the piece to promote to (`PType::NoneType` if no promotion).
    bool capture {false};           // Capture flag. 
    bool double_push {false};       // Double push flag. 
    bool en_passant {false};        // En passant flag. 
    bool castle {false};            // Castle flag. 

    int score {0};
};

// Prints the move parameters to the stream in a readable format.
std::ostream& operator<<(std::ostream& out, const Move& move);

bool operator==(const Move& lhs, const Move& rhs);
bool operator!=(const Move& lhs, const Move& rhs);