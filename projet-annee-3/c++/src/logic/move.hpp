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

    uint8_t source               ;    // The source position of the move (where the piece comes from).
    uint8_t target               ;    // The target position of the move (where the piece is going).
    Player player                ;    // The `Player` of the piece that moved. 
    PType p_type                 ;    // The `PType` of the piece that moved. 
    PType promotion              ;    // The `PType` of the piece to promote to (`PType::NoneType` if no promotion).
    bool capture                 ;    // Capture flag. 
    bool double_push             ;    // Double push flag. 
    bool en_passant              ;    // En passant flag. 
    bool castle                  ;    // Castle flag. 

    int score {0};

    auto operator<=>(const Move&) const = default;
};

// Prints the move parameters to the stream in a readable format.
std::ostream& operator<<(std::ostream& out, const Move& move);