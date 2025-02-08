#pragma once

#include "player.hpp"
#include "position.hpp"
#include "piece.hpp"

#include <cstdint>
#include <iostream>

/**
 * @brief Structure containing move parameters.
 * 
 * Bitfields? In *my* modern C++ application? It's more likely than you'd think.
 */
struct Move {
    uint8_t source               : 6;    // The source position of the move (where the piece comes from).
    uint8_t target               : 6;    // The target position of the move (where the piece is going).
    Player player                : 1;    // The `Player` of the piece that moved. 
    Piece::Type p_type           : 3;    // The `Piece::Type` of the piece that moved. 
    Piece::Type promotion        : 4;    // The `Piece::Type` of the piece to promote to (`Piece::NoneType` if no promotion).
    bool capture                 : 1;    // Capture flag. 
    bool double_push             : 1;    // Double push flag. 
    bool en_passant              : 1;    // En passant flag. 
    bool castle                  : 1;    // Castle flag. 
};

// Prints the move parameters to the stream in a readable format.
std::ostream& operator<<(std::ostream& out, const Move& move);